#include "../SmartCompressorProj/Source/audioCompression.hpp"
#include <algorithm>

AudioCompression::AudioCompression() {
    targetMaxVolume = 0;
    targetMinVolume = 0;
    targetAvgVolume = 0;
    maxAdjustment = 0;
    compressionSpeed = 0;
    silenceThreshold = 0;
}

AudioCompression::AudioCompression(std::vector<std::pair<float, float>>& setAudio,
    float userMaxVol,
    float userMinVol,
    float userAvgVol,
    float setMaxAdj,
    size_t setComprSpd,
    float setSilenceThres) : audio_(setAudio), targetMaxVolume(userMaxVol), targetMinVolume(userMinVol), targetAvgVolume(userAvgVol), maxAdjustment(setMaxAdj), compressionSpeed(setComprSpd), silenceThreshold(setSilenceThres) {

}

AudioCompression::AudioCompression(std::vector<std::pair<float, float> >& audio) {
    audio_ = audio;
    // also take in goals set by the user? change to fit GUI
    getUserSpecs();
}

void AudioCompression::getUserSpecs(){
    std::cout << "Enter Maximum Volume: ";
    std::cin >> targetMaxVolume; 
    std::cout << "Enter Minimum Volume: ";
    std::cin >> targetMinVolume; // minimum peak volume above the silence threshold? 
    std::cout << "Enter Average Volume: ";
    std::cin >> targetAvgVolume;
    std::cout << "Enter Maximum Adjustment Factor: ";
    std::cin >> maxAdjustment;
    std::cout << "Enter Compression Speed: ";
    std::cin >> compressionSpeed; // this is a measure of how many samples to take into account at the same time
    std::cout << "Enter Silence Threshold: ";
    std::cin >> silenceThreshold;
}

float AudioCompression::getMaxVolume(){
    return targetMaxVolume;
}

float AudioCompression::getMinVolume(){
    return targetMinVolume;
}

float AudioCompression::getAvgVolume(){
    return targetAvgVolume;
}

float AudioCompression::threshold() {
    return silenceThreshold;
}

std::vector<std::pair<float, float> > AudioCompression::getAudio(){
    return audio_;
}

bool AudioCompression::amplify(size_t index, float factor) {
    if (factor < 0) return false;
    if (audio_[index].first * factor > 1 || audio_[index].second * factor > 1) 
        return false; // prevents clipping
    audio_[index].first *= factor;
    audio_[index].second *= factor;
    return true;
}

float AudioCompression::max(size_t start, size_t end) {
    float output = 0;
    for (size_t i = start; i < end; i++) {
        if (std::abs(audio_[i].first) > output) output = std::abs(audio_[i].first);
        if (std::abs(audio_[i].second) > output) output = std::abs(audio_[i].second);
    }
    return output;
}

bool AudioCompression::isPeak(size_t index) {
    size_t start = std::max((size_t)0, index - (compressionSpeed / 2));
    size_t end = std::min(audio_.size(), index + (compressionSpeed / 2)); // TODO: redefine detection range
    float m = max(start, end);
    return std::abs(audio_[index].first) == m || std::abs(audio_[index].second) == m;
}

float AudioCompression::minPeak(size_t start, size_t end) {
    float min = audio_[start].first;
    for (size_t i = start + 1; i < end - 1; i++) {
        if (isPeak(i) && !silence(i)) {
            if (audio_[i].first < min) {
                min = audio_[i].first;
            }
            if (audio_[i].second < min) {
                min = audio_[i].second;
            }
        }
    }
    return min;
}

std::pair<float, float> AudioCompression::averagePeak(size_t start, size_t end) {
    std::pair<float, float> total;
    total.first = 0;
    total.second = 0;
    int count = 0;
    for (size_t i = start + 1; i < end - 1; i++) {
        if (audio_[i].first > audio_[i - 1].first && audio_[i].first > audio_[i + 1].first) {
            total.first += audio_[i].first;
            total.second += audio_[i].second;
            count++;
        }
    }
    if (count == 0) return total;
    total.first /= count;
    total.second /= count;
    return total;
}

bool AudioCompression::silence(size_t index) {
    double mx;
    if (compressionSpeed > audio_.size()) {
        mx = max(0, audio_.size());
    }
    size_t start = std::max(index - compressionSpeed, (size_t)0);
    size_t end = std::min(index + compressionSpeed, audio_.size());
    mx = max(start, end);
    return mx < silenceThreshold;
}

void AudioCompression::compress(size_t start, size_t end) {
    std::vector<double> factors;
    size_t cSpeed = std::min(compressionSpeed, end - start);
    for (size_t i = start; i < end; i++) {
        size_t st = std::max(start, i - cSpeed);
        size_t en = std::min(end, i + cSpeed);
        std::pair<double, double> avg = averagePeak(st, en);
        if (silence(i)) {
            factors.push_back(1);
            continue;
        }
        double factor = targetAvgVolume / std::max(avg.first, avg.second);
        double currentMax = max(st, en);
        if (currentMax * factor > 1) {
            factor = 1 / currentMax;
        }
        float larger = std::max(std::abs(audio_[i].first), std::abs(audio_[i].second));
        if (factor > 1 && larger * factor - larger > maxAdjustment) {
            factor = maxAdjustment / larger + 1;
        }
        if (factor < 1 && larger - larger * factor > maxAdjustment) {
            factor = 1 - maxAdjustment / larger;
        }
        factors.push_back(factor);
    }
    for (size_t i = start; i < end; i++) {
        bool success = amplify(i, factors[i - start]);
        if (!success) {
            std::cout << "Unsuccessful amplification at index " << i << std::endl;
        }
    }
    fitToRange(start, end);
}

void AudioCompression::fitToRange(size_t start, size_t end) {
    float mx = max(start, end);
    float min = minPeak(start, end);
    float factor = (targetMaxVolume - targetMinVolume) / (mx - min);
    float targetMed = targetMaxVolume - ((targetMaxVolume - targetMinVolume) / 2);
    float currMed = mx - ((mx - min) / 2);
    float difference = targetMed - currMed;
    for (size_t i = start; i < end; i++) {
        if (!silence(i)) {
            audio_[i].first += difference;
            audio_[i].second += difference; // TODO: unbork order of operations
            bool success = amplify(i, factor);
            if (!success) {
                std::cout << "Unsuccessful amplification at index " << i << std::endl;
            }
        }
    }
}
