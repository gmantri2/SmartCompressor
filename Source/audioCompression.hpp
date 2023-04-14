#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

class AudioCompression{
    public:
        /**
        * Default constructor
        */
        AudioCompression();

        /**
        * Constructor to fill in values.
        * @param setAudio Audio data to load in
        * @param userMaxVol User Maximum volume
        * @param userMinVol User Minimum volume
        * @param userAvgVol User Average volume
        * @param setMaxAdj Set Maximum adjustment
        * @param setComprSpd Set Compression speed
        * @param setSilenceThres Set Silence threshold
        */
        AudioCompression(std::vector<std::pair<float, float>>& setAudio,
            float userMaxVol,
            float userMinVol,
            float userAvgVol,
            float setMaxAdj,
            size_t setComprSpd,
            float setSilenceThres);

        AudioCompression(std::vector<std::pair<float,float> >& audio);
        void getUserSpecs();
        float getMaxVolume();
        float getMinVolume();
        float getAvgVolume();
        float threshold();
        std::vector<std::pair<float,float> > getAudio();
        size_t size() {return audio_.size();}
        /**
         * Amplifies a single data point.
         * @return whether amplification was successful
         */
        bool amplify(size_t index, float factor);
        /**
         * Returns the largest magnitude of a value among all selected data points.
         */
        float max(size_t start, size_t end);
        /**
         * Returns whether the specified point is at a peak, used to determine actual volume. 
         */
        bool isPeak(size_t index);
        /**
         * Returns the minimum peak amplitude, excluding those below the silence threshold. 
         */
        float minPeak(size_t start, size_t end);
        /**
         * Returns the average peak amplitude of the specified section of the audio. 
         * @return the average peak amplitude in stereo 
         */
        std::pair<float, float> averagePeak(size_t start, size_t end);
        /**
         * Returns whether the point is in silence as specified by the silence threshold.
         */
        bool silence(size_t index);
        /**
         * Compresses the specified section of the audio; may be split
         * into separate parts as the compression algorithm grows complex.
         */
        void compress(size_t start, size_t end);
        /**
         * Scales the specified section by a constant amount to fit within 
         * the min to max range, excluding points beneath the silence threshold.
         */
        void fitToRange(size_t start, size_t end);
        
    private:
        std::vector<std::pair<float,float> > audio_;
        float targetMaxVolume;
        float targetMinVolume;
        float targetAvgVolume;
        float maxAdjustment;
        size_t compressionSpeed;
        float silenceThreshold;
    
    /** NOTES
     * - Compressing the two sides of stereo audio separately would likely result in some loss of
     * stereo contrast. So far, the code compresses both sides equally. 
     * - Might want to get rid of target average volume in favor of min and max. 
     * - Code currently assumes that the data points are within [0, 1). 
     * - Are there concerns of the compression changing the shape of the wave? Probably if compression 
     * speed is too narrow? In which case, what is an appropriate minimum compressionSpeed value? 
     */
};