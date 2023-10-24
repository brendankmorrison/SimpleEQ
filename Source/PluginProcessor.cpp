/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessor::SimpleEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameterLayout()), forwardFFT(fftOrder), window(fftSize, juce::dsp::WindowingFunction<float>::hann)
                        
#endif
{
    //CircularBuffer = juce::AudioBuffer<float>(getTotalNumInputChannels(), 1024);
}

SimpleEQAudioProcessor::~SimpleEQAudioProcessor()
{
    //fft = std::make_unique<juce::dsp::FFT>(8);
}

//==============================================================================
const juce::String SimpleEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

template<typename ChainType, typename CoefficientType>
void SimpleEQAudioProcessor::updateCoefficients(ChainType& chain, CoefficientType& coefficients, int slope)
{
        chain.template setBypassed<0>(true);
        chain.template setBypassed<1>(true);
        chain.template setBypassed<2>(true);
        chain.template setBypassed<3>(true);
    
        //*chain.template get<0>().coefficients = *coefficients[0];
        chain.template setBypassed<0>(false);

        switch (slope)
        {
            case Slope_12:
            {
                *chain.template get<0>().coefficients = *coefficients[0];
                chain.template setBypassed<0>(false);
                break;
            }
            case Slope_24:
            {
                *chain.template get<0>().coefficients = *coefficients[0];
                chain.template setBypassed<0>(false);
                *chain.template get<1>().coefficients = *coefficients[1];
                chain.template setBypassed<1>(false);
                break;
            }
            case Slope_36:
            {
                *chain.template get<0>().coefficients = *coefficients[0];
                chain.template setBypassed<0>(false);
                *chain.template get<1>().coefficients = *coefficients[1];
                chain.template setBypassed<1>(false);
                *chain.template get<2>().coefficients = *coefficients[2];
                chain.template setBypassed<2>(false);
                break;
            }
            case Slope_48:
            {
                *chain.template get<0>().coefficients = *coefficients[0];
                chain.template setBypassed<0>(false);
                *chain.template get<1>().coefficients = *coefficients[1];
                chain.template setBypassed<1>(false);
                *chain.template get<2>().coefficients = *coefficients[2];
                chain.template setBypassed<2>(false);
                *chain.template get<3>().coefficients = *coefficients[3];
                chain.template setBypassed<3>(false);
                break;
            }
        }
    }

//==============================================================================
void SimpleEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    juce::dsp::ProcessSpec spec;
    
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    
    leftChannel.prepare(spec);
    rightChannel.prepare(spec);
    
    auto chainSettings = getChainSettings(apvts);
    
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                                                                                sampleRate,
                                                                                chainSettings.peakFreq,
                                                                                chainSettings.peakQuality,
                                                                                juce::Decibels::decibelsToGain(chainSettings.peakGainDb));
    
    *leftChannel.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
    *rightChannel.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
    
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq, getSampleRate(), 2 * (chainSettings.lowCutSlope + 1));
    
    auto& leftLowCut = leftChannel.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChannel.get<ChainPositions::LowCut>();
    
    updateCoefficients(leftLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
    updateCoefficients(rightLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
    
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq, getSampleRate(), 2 * (chainSettings.highCutSlope + 1));
    
    auto& leftHighCut = leftChannel.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChannel.get<ChainPositions::HighCut>();
    
    updateCoefficients(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCoefficients(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
}

void SimpleEQAudioProcessor::pushNextSampleIntoFifo (float sample) noexcept
    {
        // if the fifo contains enough data, set a flag to say
        // that the next frame should now be rendered..
        if (fifoIndex == fftSize)               // [11]
        {
            if (! nextFFTBlockReady)            // [12]
            {
                juce::zeromem (fftData, sizeof (fftData));
                memcpy (fftData, fifo, sizeof (fifo));
                nextFFTBlockReady = true;
            }
 
            fifoIndex = 0;
        }
 
        fifo[fifoIndex++] = sample;             // [12]
    }

void SimpleEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    auto chainSettings = getChainSettings(apvts);
    
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                                                                                getSampleRate(),
                                                                                chainSettings.peakFreq,
                                                                                chainSettings.peakQuality,
                                                                                juce::Decibels::decibelsToGain(chainSettings.peakGainDb));
    
    *leftChannel.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
    *rightChannel.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
    
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq, getSampleRate(), 2 * (chainSettings.lowCutSlope + 1));
    
    auto& leftLowCut = leftChannel.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChannel.get<ChainPositions::LowCut>();
    
    updateCoefficients(leftLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
    updateCoefficients(rightLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
    
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq, getSampleRate(), 2 * (chainSettings.highCutSlope + 1));
    
    auto& leftHighCut = leftChannel.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChannel.get<ChainPositions::HighCut>();
    
    updateCoefficients(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCoefficients(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
    
    juce::dsp::AudioBlock<float> block(buffer);
    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    
    leftChannel.process(leftContext);
    rightChannel.process(rightContext);
    

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            if(channel == 0)
            {
                pushNextSampleIntoFifo(channelData[sample]);
            }
            
            //building a little buffer of 256 samples, taking the highest peak of that buffer and assigning its value to mAmplitude
            float singleSample = std::abs(buffer.getSample(channel, sample));
            
            if (singleSample > mMaxPeak)
                mMaxPeak = singleSample;
            
            ++mSampleCounter;
            
            if (mSampleCounter == mDetectionLength)
            {
                mAmplitude = mMaxPeak;
                mMaxPeak = 0.0;
                mSampleCounter = 0;
            }
                
        }

        // ..do something to the data...
    }
}

//==============================================================================
bool SimpleEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleEQAudioProcessor::createEditor()
{
    return new SimpleEQAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings chainSettings;
    chainSettings.peakFreq = apvts.getRawParameterValue("Peak Freq")->load();
    chainSettings.peakGainDb = apvts.getRawParameterValue("Peak Gain")->load();
    chainSettings.peakQuality = apvts.getRawParameterValue("Peak Quality")->load();
    
    chainSettings.lowCutFreq = apvts.getRawParameterValue("LowCut Freq")->load();
    chainSettings.highCutFreq = apvts.getRawParameterValue("HighCut Freq")->load();
    
    chainSettings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("LowCut Slope")->load());
    chainSettings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("HighCut Slope")->load());
    
    return chainSettings;
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleEQAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    auto pLowCutFreq = std::make_unique<juce::AudioParameterFloat>(
                                                                   juce::ParameterID("LowCut Freq", 1),
                                                                   "LowCut Freq",
                                                                   juce::NormalisableRange<float>(20.f, 20000.f, 1.f, .5f), 20.f);
    
    auto pHighCutFreq = std::make_unique<juce::AudioParameterFloat>(
                                                                    juce::ParameterID("HighCut Freq", 1),
                                                                    "HighCut Freq",
                                                                    juce::NormalisableRange<float>(20.f, 20000.f, 1.f, .5f), 20000.f);

    auto pPeakFreq = std::make_unique<juce::AudioParameterFloat>(
                                                                 juce::ParameterID("Peak Freq", 1),
                                                                 "Peak Freq",
                                                                 juce::NormalisableRange<float>(20.f, 20000.f, 1.f, .5f), 750.f);

    auto pPeakGain = std::make_unique<juce::AudioParameterFloat>(
                                                                 juce::ParameterID("Peak Gain", 1),
                                                                 "Peak Gain",
                                                                 juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f), 0.f);

    auto pPeakQuality = std::make_unique<juce::AudioParameterFloat>(
                                                                    juce::ParameterID("Peak Quality", 1),
                                                                    "Peak Quality",
                                                                    juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), 1.f);

    juce::StringArray stringArray;
    for (int i = 0; i < 4; i++)
    {
        juce::String str;
        str << (12 + i*12);
        str << " db/oct";
        stringArray.add(str);
    }

    auto pLowCutSlope = std::make_unique<juce::AudioParameterChoice>(
                                                                     juce::ParameterID("LowCut Slope", 1),
                                                                     "LowCut Slope",
                                                                     stringArray,
                                                                     0);
    auto pHighCutSlope = std::make_unique<juce::AudioParameterChoice>(
                                                                      juce::ParameterID("HighCut Slope", 1),
                                                                      "HighCut Slope",
                                                                      stringArray,
                                                                      0);
    
    params.push_back(std::move(pLowCutFreq));
    params.push_back(std::move(pHighCutFreq));
    params.push_back(std::move(pPeakFreq));
    params.push_back(std::move(pPeakGain));
    params.push_back(std::move(pPeakQuality));
    params.push_back(std::move(pLowCutSlope));
    params.push_back(std::move(pHighCutSlope));
    
    
    
    return {params.begin(), params.end()};
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQAudioProcessor();
}
