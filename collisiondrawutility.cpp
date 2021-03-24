#include <iostream>
#include <string>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdlib>
#include <cmath>
#include <cstdio>

int intsize(int k){
    int i = 1;
    while ((int) (k / 10) != 0){
        k = (int) (k / 10);
        i++;
    }
    return i;
}

unsigned long long int np_col_arr[256][256]{0};
unsigned long long int old_col_arr[256][256]{0};
unsigned long long int summs[256]{0};
double old_fr_arr[256][256]{0};
double zscores[256][256]{0};
double fr_arr[256][256]{0};
double stdev[256];
unsigned long long int CHVAL = 2251799813685000u, CURVAL=0; // ~2^51

template <typename T>
void std_dev(T arr[256][256], bool stdsum=false){
    // sqrt( sum(x-mean)^2 / N )
    double m_sum, mean_v, summ;
    for (int i=0; i < 256; i++){
        m_sum = 0;
        if (stdsum){ summ = 1.0; }
        else{ summ = summs[i]; }
        mean_v = summ / 256.0;
        for (int j=0; j < 256; j++){
            m_sum += (double)(arr[i][j] - mean_v) * (double)(arr[i][j] - mean_v);
        }
        stdev[i] = std::sqrt(m_sum / 256.0);
    }
}

template <typename T>
void standardize(T arr[256][256], bool stdsum=false){
    double mn;
    for (int i=0; i < 256; i++){
        if (stdsum){ mn = 1.0 / 256.0; }
        else{ mn = summs[i] / 256.0; }
        for (int j=0; j<256; j++){
            zscores[i][j] = (arr[i][j] - mn) / stdev[i];
        }
    }
}

unsigned long long wrts = 0, updts = 0, mxx=0;

int main(){
    std::string path_, path, cvpath;
    path_ = "D:\\rick and morty\\S3\\Rick.and.Morty.S03E01.1080p.BluRay.Farda.DL.mkv";
    //path_ = "C:\\Users\\Anil Myne\\Desktop\\desktop\\Lorn - Soft Room.mp4";
    std::cout << "___COLLISION DRAW UTILITY___";
    std::cout << "\nENTER IMAGE PATH: ";
    std::getline(std::cin, path_);
    for (auto &chr : path_){ if (chr != '"'){ path.push_back(chr); } }

    cv::Mat imgg = cv::imread(path, 0);
    cv::Mat blank = cv::Mat(imgg.rows, imgg.cols, CV_8U, cv::Scalar(0));

    if(imgg.empty()){
        std::cout << "\nInvalid Image File";
        return 0;
    }

    unsigned char prev, up, next, down, curr;
    bool setcurr;

    for (int y=1; y<imgg.rows -1; y++){
        for (int x=1; x < imgg.cols-1;x++){
            prev = imgg.ptr<unsigned char>(y)[x-1];
            up = imgg.ptr<unsigned char>(y-1)[x];
            curr = imgg.ptr<unsigned char>(y)[x];
            next =  imgg.ptr<unsigned char>(y)[x+1];
            down = imgg.ptr<unsigned char>(y+1)[x];

            if ((prev != curr) || (down != curr)){
                blank.ptr<unsigned char>(y)[x] = 255;
            }
        }
    }

    cv::namedWindow("IMAGE", cv::WINDOW_NORMAL);
    cv::imshow("IMAGE", blank);
    cv::waitKey();

}
