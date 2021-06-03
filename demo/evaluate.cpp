#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

#include "DBoW2.h"

using namespace std;
using namespace cv;
using namespace DBoW2;

typedef vector<vector<Mat>> Features;

vector<string> LoadImagesPath(const string &sequencePath,
                              const int &nImages, const string &fPrefix, const int &fZeros);
void loadFeatures(Features &features, vector<string> &_vPathToImages);
void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out);
void databaseQueries(const string _vocPath, const Features &_features);

vector<string> LoadImagesPath(const string &sequencePath,
                              const int &nImages, const string &fPrefix, const int &fZeros)
{
    string prefixFilePath = sequencePath + "/" + fPrefix;

    vector<string> vPathToImage;
    vPathToImage.resize(nImages);

    for (int i = 0; i < nImages; i++)
    {
        stringstream imgFileName;
        imgFileName << setfill('0') << setw(fZeros) << i;
        vPathToImage[i] = prefixFilePath + imgFileName.str() + ".png";
    }
    return vPathToImage;
}

Features loadFeatures(vector<string> &_vPathToImages)
{
    Features features;
    features.clear();

    int _size = (int)_vPathToImages.size();
    features.reserve(_size);

    cv::Ptr<cv::ORB> orb = cv::ORB::create();

    cout << "Extracting ORB features..." << endl;

    for (auto &&i : _vPathToImages)
    {
        cv::Mat image = cv::imread(i, 0);

        if (image.empty())
        {
            throw std::runtime_error("Failed to load image at: " + string(i));
        }
        cv::Mat mask;
        vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;

        orb->detectAndCompute(image, mask, keypoints, descriptors);

        features.push_back(vector<cv::Mat>());
        changeStructure(descriptors, features.back());
    }
    return features;
}

void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out)
{
    out.resize(plain.rows);

    for (int i = 0; i < plain.rows; ++i)
    {
        out[i] = plain.row(i);
    }
}

// void vocCreation(const string &_vocPath, const Features &_features,
//                  int k = 9, int L = 3, WeightingType weight = TF_IDF, ScoringType scoring = L1_NORM)
// {
//     OrbVocabulary voc(k, L, weight, scoring);
//     cout << "Creating a " << k << "^" << L << " vocabulary..." << endl;
//     voc.create(_features);
//     cout << "Done!" << endl;
//     cout << "Vocabulary information: " << endl
//          << voc << endl
//          << endl;

//     cout << "Saving vocabulary..." << endl;
//     voc.save(_vocPath);
//     cout << "Done!" << endl;
// }

void vocCreation(const string & vocPath, const Features & features)
{
    OrbVocabulary voc(vocPath);
    voc.create(features);
    voc.save(vocPath);
}

void databaseQueries(const string _dbPath, const Features &_features)
{
    cout << "Loading the database..." << endl;
    // OrbVocabulary voc(_vocPath);
    OrbDatabase db(_dbPath);
    cout << "Done!" << endl;

    // cout << "Adding Image to database..." << endl;
    // for (int i = 0; i < (int)_features.size(); i++)
    // {
    //     db.add(_features[i]);
    // }
    // cout << "Done!" << endl;

    cout << "Database information: " << endl
         << db << endl;
    cout << "Querying the database..." << endl;
    QueryResults results;

    for (int i = 0; i < (int)_features.size(); i++)
    {
        db.query(_features[i], results, 4);
        cout << "Searching for Image " << i << ". " << results << endl;
    }

    cout << "Saving database..." << endl;
    db.save(_dbPath);
    cout << "Done!" << endl;
}dbow                      | print help message         }"
        "{@sequence_path | /media/rana/0457451508/rana-data/masters/Dataset/kitti-original/grayscale/sequences/01/image_0                   | path to sequence           }"
        "{@db_path       | small_db.yml.gz          | path to database           }"
        "{voc_path       | small_voc.yml.gz         | path to vocabulary         }"
        "{n              | 1000                        | number of images           }"
        "{p              | <none>                   | file name prefix           }"
        "{z              | 6                        | number of zeros to filename}"};

int main(int argc, char const *argv[])
{
    try
    {
        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("ORB Feature Evaluation v1.0.0");

        const string sequencePath = parser.get<string>("@sequence_path");
        const string databasePath = parser.get<string>("@db_path");
        const string vocabularyPath = parser.get<string>("voc_path");
        const string fPrefix = parser.get<string>("p");
        const int nImages = parser.get<int>("n");
        const int fZeros = parser.get<int>("z");

        if (parser.has("help"))
        {
            parser.printMessage();
            return 0;
        }

        auto vPathToImages = LoadImagesPath(sequencePath, nImages, fPrefix, fZeros);

        // tic
        // auto t_start = chrono::high_resolution_clock::now();

        auto features = loadFeatures(vPathToImages);

        // auto t_end = chrono::high_resolution_clock::now();

        // double timestamp = double (t_end - t_start);

        // toc
        // times.pushback(currentTimer);

        // vocCreation(vocabularyPath, features);

        // tic
        // databaseQueries(databasePath, features);
        // toc
        // queryTime.pushback(currentTimer)

        // auto score = databaseQueries(databasePath, features);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}