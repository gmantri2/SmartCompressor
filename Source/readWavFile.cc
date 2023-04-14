#include "../SmartCompressorProj/Source/readWavFile.hpp"

vector<float> readWav(const char* fileName) {
    long fileSize;

    FILE* filestream0 =fopen(fileName,"rb");
    if(!filestream0) {
        cout << "failed to open wav file!" << endl;
        exit(1);
    }

    fseek (filestream0, 0, SEEK_END);
    fileSize = ftell(filestream0);
    fclose(filestream0);
    FILE* filestream =fopen(fileName,"rb");

    short int* filedata= new short[fileSize / sizeof(short int)];
    fread(filedata, sizeof(short int), fileSize / sizeof(short int), filestream);
    fclose(filestream);

    vector<float> result;
    for (int i = (44 / sizeof(short int)) + 10; i < (int)(fileSize / sizeof(short int)); i++) {
        result.push_back(filedata[i] / 32768.0);
    }

    delete[] filedata;

    return result;

    /*
    //how to run python code in c++
    char filename[] = "WavtoCsvText.py";
    FILE* fp;

    Py_Initialize();

    fp = _Py_fopen(filename, "r");
    PyRun_SimpleFile(fp, filename);

    Py_Finalize();
    */
}

vector<pair<float,float> > convertToPairs(vector<float> result){
    vector<pair<float,float> > ret;
    unsigned x = 0;
    while(x < result.size()){
        float one = result[x];
        float two = result[x+1];
        pair<float,float> p;
        p.first = one;
        p.second = two;
        ret.push_back(p);
        x += 2;
    }
    return ret;
}

vector<float> convertBack(vector<pair<float,float> > compressed){
    vector<float> ret;
    for(size_t x = 0;x<compressed.size();x++){
        pair<float,float> p = compressed[x];
        ret.push_back(p.first);
        ret.push_back(p.second);
    }
    return ret;
}

void vectToWav(vector<float> input, const char* fileName, const char* newFileName) {
    std::string temp = fileName;
    long fileSize;

    FILE* filestream =fopen(fileName,"rb");

    if(!filestream) {
        cout << "failed to open wav file!" << " " << errno << endl;
        exit(1);
    }


    fseek (filestream, 0, SEEK_END);
    fileSize = ftell(filestream);
    fclose(filestream);
    filestream = fopen(fileName,"rb+");


    short int* filedata= new short[fileSize / sizeof(short int)];
    fread(filedata, sizeof(short int), fileSize / sizeof(short int), filestream);

    for (size_t i = 0; i < input.size(); i++) {
        filedata[i + (44 / sizeof(short int)) + 10] = input[i] * 32768.0;
    }


    fclose(filestream);
    filestream = fopen(newFileName,"wb+");
    fwrite(filedata, sizeof(short int), fileSize / sizeof(short int), filestream);
    fclose(filestream);

    delete[] filedata;
}
