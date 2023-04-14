#pragma once
#include "readWavFile.hpp"
#include "audioCompression.hpp"
#include <time.h>

//==============================================================================
class MainContentComponent   : public juce::AudioAppComponent,
                               public juce::ChangeListener,
                               private juce::Timer
{
public:
    MainContentComponent()
        : state (Stopped),
          thumbnailCache(5),
          thumbnail (512, formatManager, thumbnailCache),
          thumbnailCacheOld(5),
          thumbnailOld (512, formatManager, thumbnailCache)
    {
        AudioCompression au;
        //BUTTONS & VOLUME SLIDER
        
        addAndMakeVisible (&openButton);
        openButton.setButtonText ("Open...");
        openButton.onClick = [this] { openButtonClicked(); };

        addAndMakeVisible (&playButton);
        playButton.setButtonText ("Play");
        playButton.onClick = [this] { playButtonClicked(); };
        playButton.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
        playButton.setEnabled (false);

        addAndMakeVisible (&stopButton);
        stopButton.setButtonText ("Stop");
        stopButton.onClick = [this] { stopButtonClicked(); };
        stopButton.setColour (juce::TextButton::buttonColourId, juce::Colours::red);
        stopButton.setEnabled (false);
        
        addAndMakeVisible (&convertButton);
        convertButton.setButtonText ("Compress!");
        convertButton.onClick = [this] { convertButtonClicked(); };
        convertButton.setColour (juce::TextButton::buttonColourId, juce::Colours::red);
        convertButton.setEnabled (true);
        
        addAndMakeVisible (&midiVolume);
        midiVolume.setSliderStyle (juce::Slider::LinearBarVertical);
        midiVolume.setRange (0.0, 100.0, 1.0);
        midiVolume.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
        midiVolume.setPopupDisplayEnabled (true, false, this);
        midiVolume.setTextValueSuffix (" Volume");
        midiVolume.setValue(1.0);
        
        //USER SPECS
        
        addAndMakeVisible (maxVolume);
        maxVolume.setText ("Maximum Volume:", juce::dontSendNotification);
        maxVolume.attachToComponent (&maxVolumeText, true);
        maxVolume.setColour (juce::Label::textColourId, juce::Colours::orange);
        maxVolume.setJustificationType (juce::Justification::right);
        
        addAndMakeVisible (maxVolumeText);
        maxVolumeText.setEditable (true);
        maxVolumeText.setColour (juce::Label::backgroundColourId, juce::Colours::darkblue);
        
        addAndMakeVisible (minVolume);
        minVolume.setText ("Minimum Volume:", juce::dontSendNotification);
        minVolume.attachToComponent (&minVolumeText, true);
        minVolume.setColour (juce::Label::textColourId, juce::Colours::orange);
        minVolume.setJustificationType (juce::Justification::right);
        
        addAndMakeVisible (minVolumeText);
        minVolumeText.setEditable (true);
        minVolumeText.setColour (juce::Label::backgroundColourId, juce::Colours::darkblue);
        
        addAndMakeVisible (avgVolume);
        avgVolume.setText ("Average Volume:", juce::dontSendNotification);
        avgVolume.attachToComponent (&avgVolumeText, true);
        avgVolume.setColour (juce::Label::textColourId, juce::Colours::orange);
        avgVolume.setJustificationType (juce::Justification::right);
        
        addAndMakeVisible (avgVolumeText);
        avgVolumeText.setEditable (true);
        avgVolumeText.setColour (juce::Label::backgroundColourId, juce::Colours::darkblue);
        
        addAndMakeVisible (maxAdjustment);
        maxAdjustment.setText ("Maximum Adjustment :", juce::dontSendNotification);
        maxAdjustment.attachToComponent (&maxAdjustmentText, true);
        maxAdjustment.setColour (juce::Label::textColourId, juce::Colours::orange);
        maxAdjustment.setJustificationType (juce::Justification::right);
        
        addAndMakeVisible (maxAdjustmentText);
        maxAdjustmentText.setEditable (true);
        maxAdjustmentText.setColour (juce::Label::backgroundColourId, juce::Colours::darkblue);
        
        addAndMakeVisible (compressionSpeed);
        compressionSpeed.setText ("Compression Speed :", juce::dontSendNotification);
        compressionSpeed.attachToComponent (&compressionSpeedText, true);
        compressionSpeed.setColour (juce::Label::textColourId, juce::Colours::orange);
        compressionSpeed.setJustificationType (juce::Justification::right);
        
        addAndMakeVisible (compressionSpeedText);
        compressionSpeedText.setEditable (true);
        compressionSpeedText.setColour (juce::Label::backgroundColourId, juce::Colours::darkblue);
        
        addAndMakeVisible (silenceThreshold);
        silenceThreshold.setText ("Silence Threshold :", juce::dontSendNotification);
        silenceThreshold.attachToComponent (&silenceThresholdText, true);
        silenceThreshold.setColour (juce::Label::textColourId, juce::Colours::orange);
        silenceThreshold.setJustificationType (juce::Justification::right);
        
        addAndMakeVisible (silenceThresholdText);
        silenceThresholdText.setEditable (true);
        silenceThresholdText.setColour (juce::Label::backgroundColourId, juce::Colours::darkblue);
        
        //APP SIZE

        addAndMakeVisible(&durationLabel, 0);
        durationLabel.setFont(juce::Font(16.0f, juce::Font::bold));
        durationLabel.setText("Duration", juce::dontSendNotification);
        durationLabel.setHelpText("Length");
        durationLabel.setColour(juce::Label::textColourId, juce::Colours::blue);
        durationLabel.setColour(juce::Label::backgroundColourId, juce::Colours::orange);
        durationLabel.setJustificationType(juce::Justification::centred);
        //Label("Duration", "0:00");

        addAndMakeVisible(&exportButton, 0);
        exportButton.setButtonText("Export to File");
        exportButton.onClick = [this] { exportButtonClicked(); };
        exportButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
        exportButton.setEnabled(true);

        addAndMakeVisible(&exportLabel, 0);
        exportLabel.setFont(juce::Font(16.0f, juce::Font::bold));
        exportLabel.setText("Location", juce::dontSendNotification);
        exportLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        exportLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkgreen);
        exportLabel.setJustificationType(juce::Justification::centred);
        exportLabel.setEditable(true);
        //exportLabel.onTextChange = [this] {}


        setSize (1500, 750);

        formatManager.registerBasicFormats();       // [1]
        transportSource.addChangeListener (this);   // [2]
        thumbnail.addChangeListener (this);
        thumbnailOld.addChangeListener (this);
        startTimer(40);

        setAudioChannels (0, 2);
    
    }

    ~MainContentComponent() override
    {
        shutdownAudio();
        stopTimer();
    }

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
    }

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        if (readerSource.get() == nullptr)
        {
            bufferToFill.clearActiveBufferRegion();
            return;
        }

        transportSource.getNextAudioBlock (bufferToFill);
        
        auto level = (float) midiVolume.getValue()/4;

            for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
            {
                auto* buffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);

                for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    buffer[sample]  = buffer[sample] * level;
                }
            }
    }

    void releaseResources() override
    {
        transportSource.releaseResources();
    }

    void resized() override
    {
        openButton.setBounds (10, 40, getWidth() - 20, 20);
        playButton.setBounds (10, 70, getWidth() - 20, 20);
        stopButton.setBounds (10, 100, getWidth() - 20, 20);
        convertButton.setBounds (100, 670, 90, 50);
        midiVolume.setBounds (10, 130, 20, getHeight() - 150);
        
        maxVolumeText.setBounds (190, 150, 75, 25);
        minVolumeText.setBounds (190, 240, 75, 25);
        avgVolumeText.setBounds (190, 330, 75, 25);
        maxAdjustmentText.setBounds (190, 420, 75, 25);
        compressionSpeedText.setBounds (190, 510, 75, 25);
        silenceThresholdText.setBounds (190, 600, 75, 25);
        durationLabel.setBounds (280, 430, 100, 20);
        exportButton.setBounds(1340, 725, 90, 20);
        exportLabel.setBounds(280, 725, 1150, 20);
    }

    void changeListenerCallback (juce::ChangeBroadcaster* source) override
    {
        if (source == &transportSource)
        {
            if (transportSource.isPlaying())
                changeState (Playing);
            else if ((state == Stopping) || (state == Playing))
                changeState (Stopped);
            else if (Pausing == state)
                changeState (Paused);
        }
        if (source == &thumbnail){
            thumbnailChanged();
        }
        if (source == &thumbnailOld){
            thumbnailChanged();
        }
    }
    
    void paint (juce::Graphics& g) override
    {
        // fill the whole window white
        g.fillAll (juce::Colours::white);
     
        // set the current drawing colour to black
        g.setColour (juce::Colours::black);
     
        // set the font size and draw text to the screen
        g.setFont (15.0f);
     
        g.drawFittedText ("Welcome to the Smart Compressor!", 10, 5, getWidth(), 30, juce::Justification::centred, 1);
        
        juce::Rectangle<int> thumbnailBounds (280, 430, 1150, 290);
        juce::Rectangle<int> thumbnailBoundsOld (280, 130, 1150, 290);
         
        if (thumbnail.getNumChannels() == 0)
            paintIfNoFileLoaded (g, thumbnailBounds);
        else
            paintIfFileLoaded (g, thumbnailBounds);
        
        if (thumbnailOld.getNumChannels() == 0)
            paintIfNoFileLoaded (g, thumbnailBoundsOld);
        else{
            g.setColour (juce::Colours::white);
            g.fillRect (thumbnailBoundsOld);

            g.setColour (juce::Colours::red);

            auto audioLength = (float) thumbnailOld.getTotalLength();                               // [12]
            thumbnailOld.drawChannels (g, thumbnailBoundsOld, 0.0, audioLength, 1.0f);
        }
    }

private:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping,
    };

    void changeState (TransportState newState)
    {
        if (state != newState)
        {
            state = newState;

            switch (state)
            {
                case Stopped:
                    playButton.setButtonText ("Play");
                    stopButton.setButtonText ("Stop");
                    stopButton.setEnabled (false);
                    transportSource.setPosition (0.0);
                    break;
 
                case Starting:
                    transportSource.start();
                    break;
 
                case Playing:
                    playButton.setButtonText ("Pause");
                    stopButton.setButtonText ("Stop");
                    stopButton.setEnabled (true);
                    break;
 
                case Pausing:
                    transportSource.stop();
                    break;
 
                case Paused:
                    playButton.setButtonText ("Resume");
                    stopButton.setButtonText ("Return to Zero");
                    break;
 
                case Stopping:
                    transportSource.stop();
                    break;
            }
        }
    }

    void openButtonClicked()
    {
        chooser = std::make_unique<juce::FileChooser> ("Select a Wave file to play...",
                                                       juce::File{},
                                                       "*.wav");                     // [7]
        
        File f = chooser->getResult();
        // convert to vector of floats
        // audio compression
        // write back to file f as FileChooser (new chooser)
        
        auto chooserFlags = juce::FileBrowserComponent::openMode
                          | juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync (chooserFlags, [this] (const FileChooser& fc)           // [8]
        {
            auto file = fc.getResult();
            //std::vector<std::pair<double, double>> ad = _getData(str);
            if (file != File{})                                                      // [9]
            {
                // [10]
                string str = file.getFullPathName().toStdString();
                loaded_File = file.getFullPathName();
                const char* ch = str.c_str();
                auto rw = readWav(ch);
                Wav_data = rw;
                
                //auto* reader = formatManager.createReaderFor (file);
                //if (reader != nullptr)
                // {
                    // auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);   // [11]
//                    transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);       // [12]
//                    playButton.setEnabled (true);
                // [13]
                
                thumbnailOld.setSource (new juce::FileInputSource (file));
                
                    //readerSource.reset (newSource.release());                                          // [14]
                // }
            }
        });
    }

    void playButtonClicked()
    {
        if (state == Stopped) {
            changeState(Starting);
            starttime = clock();
        }
        else if (state == Paused) {
            changeState(Starting);
            starttime = clock() - clock_timer;
        }
        else if (state == Playing) {
            changeState(Pausing);
        }
    }

    void stopButtonClicked()
    {
        if (state == Paused)
            changeState (Stopped);
        else
            changeState (Stopping);
    }
    
    void convertButtonClicked()
    {
        std::vector<pair<float,float> > v = convertToPairs(Wav_data);
        
        float maxVol = std::stof(maxVolumeText.getText().toStdString());
        float minVol = std::stof(minVolumeText.getText().toStdString());
        float avgVol = std::stof(avgVolumeText.getText().toStdString());
        float maxAdj = std::stof(maxAdjustmentText.getText().toStdString());
        float cSpeed = std::stof(compressionSpeedText.getText().toStdString());
        float sThresh = std::stof(silenceThresholdText.getText().toStdString());
        
        AudioCompression ac(v,maxVol,minVol,avgVol,maxAdj,cSpeed,sThresh);
        //AudioCompression ac(v,0.7, 0.3, 0.5, 0.3, 4410, 0.02);
        ac.compress(0,v.size());
        vector<pair<float,float> > v_pairs_output = ac.getAudio();
        
        output_data = convertBack(v_pairs_output);
        
        std::string bufftemp = loaded_File.toStdString().c_str();
        bufftemp = bufftemp.substr(0,bufftemp.length()-4) + "buffer.WAV";
        const char* buffer = bufftemp.c_str();
        vectToWav(output_data,loaded_File.toStdString().c_str(),buffer);
        
        File* newFile = new File(buffer);
        string append = "_modified";
        const char* c_append = append.c_str();
        juce::String output_path = loaded_File.toStdString().c_str();
        output_path = output_path.substring(0,output_path.length()-4);
        output_path = output_path + juce::String(c_append) + newFile->getFileExtension();
        exportLabel.setText(output_path, juce::NotificationType::dontSendNotification);

        auto* reader = formatManager.createReaderFor (*newFile);
        if (reader != nullptr)
        {
            auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);   // [11]
            transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);       // [12]
            playButton.setEnabled (true);                                                      // [13]
            thumbnail.setSource (new juce::FileInputSource (*newFile));
            readerSource.reset (newSource.release());                                          // [14]
        }
        remove(buffer);
    }
    
    void thumbnailChanged()
    {
        repaint();
    }
    
    void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
    {
        g.setColour (juce::Colours::darkgrey);
        g.fillRect (thumbnailBounds);
        g.setColour (juce::Colours::white);
        g.drawFittedText ("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
    }
    
    void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
    {
        g.setColour (juce::Colours::white);
        g.fillRect (thumbnailBounds);
 
        g.setColour (juce::Colours::red);
 
        auto audioLength = (float) thumbnail.getTotalLength();                               // [12]
        thumbnail.drawChannels (g, thumbnailBounds, 0.0, audioLength, 1.0f);
 
        g.setColour (juce::Colours::green);
 
        auto audioPosition = (float) transportSource.getCurrentPosition();
        auto drawPosition = (audioPosition / audioLength) * (float) thumbnailBounds.getWidth()
                            + (float) thumbnailBounds.getX();                                // [13]
        g.drawLine (drawPosition, (float) thumbnailBounds.getY(), drawPosition,
                    (float) thumbnailBounds.getBottom(), 2.0f);                              // [14]
    }
   
    void exportButtonClicked() {
        // Create the function to export file first.
        std::string file_path = exportLabel.getText().toStdString();
        string original_path = loaded_File.toStdString();
        const char* input = original_path.c_str();
        const char* output = file_path.c_str();
        vectToWav(output_data, input, output);
        //exportTo(file_path + ".wav");
    }
    void timerCallback() override
    {
        repaint();
        if (state == Playing) {
            clock_timer = (clock() - starttime);
            durationLabel.setText(String(clock_timer*3/(float)CLOCKS_PER_SEC).substring(0,4) + " s",
                                  juce::dontSendNotification);
        }
        if (state == Stopped) {
            clock_timer = 0;
            durationLabel.setText(String(0) + " s", juce::dontSendNotification);
        }
    }


    //==========================================================================
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton convertButton;
    juce::Slider midiVolume;
    
    juce::Label maxVolume;
    juce::Label maxVolumeText;
    juce::Label minVolume;
    juce::Label minVolumeText;
    juce::Label avgVolume;
    juce::Label avgVolumeText;
    juce::Label maxAdjustment;
    juce::Label maxAdjustmentText;
    juce::Label compressionSpeed;
    juce::Label compressionSpeedText;
    juce::Label silenceThreshold;
    juce::Label silenceThresholdText;

    juce::Label durationLabel;
    juce::TextButton exportButton;
    juce::Label exportLabel;

    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    TransportState state;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    juce::AudioThumbnailCache thumbnailCacheOld;
    juce::AudioThumbnail thumbnailOld;

    juce::String loaded_File;
    vector<float> Wav_data;
    std::vector<float> output_data;
    unsigned long clock_timer = 0;
    unsigned long starttime;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
