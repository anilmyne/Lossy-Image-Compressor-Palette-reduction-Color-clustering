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

int metric_l(int x0, int y0, int x1, int y1){
    int diffx = x1 - x0, diffy = y1 - y0;
    if (diffx < 0){ diffx *= -1; }
    if (diffy < 0){ diffy *= -1; }
    return (diffx > diffy) ? diffx : diffy;
}

int metric_e(int x0, int y0, int x1, int y1){
    int diffx = x1 - x0, diffy = y1 - y0;
    return (int) std::sqrt((diffx*diffx) + (diffy*diffy));
}

unsigned char palette_cch[256]{0}, palette[256]{0};

int main(){
    std::string path_, path, cvpath;
    path_ = "D:\\nf\\pic\\a1.jpeg";
    path_ = "C:\\Users\\Anil Myne\\Desktop\\AIsets\\envir.png";
    std::cout << "___DISTANCE MARGINER___\n";
    std::cout << "Enter Image Path : ";
    std::getline(std::cin, path_);
    for (auto &chr : path_){ if (chr != '"'){ path.push_back(chr); } }

    std::cout << path << "\n";
    cv::Mat imgg = cv::imread(path, 0);
    int csize = 0;

    // palette creation
    unsigned char line_has_color[imgg.rows][256]{0};

    for (int y=0; y < imgg.rows; y++){
        for (int x=0; x < imgg.cols; x++){
            palette_cch[(int)imgg.ptr<unsigned char>(y)[x]] = 1;
            line_has_color[y][imgg.ptr<unsigned char>(y)[x]] = 1;
        }
    }
    for (int i=0; i < 256; i++){
        if (palette_cch[i] == 1){ palette[csize++] = i; }
    }

    std::cout << "\nCSIZE:" << csize;
    // std::cout << "\n"; for (int i=0; i < csize; i++){ std::cout << (int)palette[i] << ", "; } std::cout << "\n";
    int ncr = imgg.cols * imgg.rows;
    std::cout << "\nMAX/" << (imgg.cols * imgg.rows);
    int feature[csize + 1];

    int y_u, y_l, max_u, max_l;
    int dist, mat_i=0, aval = 0;

    // check for distance horizontally by moving one line away from the current
    // pixel's line radially until you find the first match
    FILE *fw = fopen("features.csv", "w");
    std::cout << "\nCOUNT : ";
    for (int y=0; y < imgg.rows; y++){
        if (mat_i % 4 == 0){ // progress view
            for (int k=0; k < aval; k++){
                std::cout << "\b";
            }
            std::cout << mat_i;
            aval = intsize(mat_i);
        }
        for (int x=0; x < imgg.cols; x++){
            for (int c=0; c < csize; c++){
                y_u = y;
                // this part finds the closest line and point(s) with requires color
                while (line_has_color[y_u][palette[c]] == 0){ // find line with color
                    y_u--;
                    if (y_u <= 0){ y_u = 0; break; }
                }

                max_u = imgg.cols * imgg.rows;
                for (int xx=x; xx >= 0; xx--){ // radiate backwards
                    if (imgg.ptr<unsigned char>(y_u)[xx] == palette[c]){
                        dist = metric_l(x, y, xx, y_u);
                        if (dist != 0 && dist < max_u){
                            max_u = dist;
                            break;
                        }
                    }
                }
                for (int xx=x; xx < imgg.cols; xx++){ // radiate backwards
                    if (imgg.ptr<unsigned char>(y_u)[xx] == palette[c]){
                        dist = metric_l(x, y, xx, y_u);
                        if (dist != 0 && dist < max_u){
                            max_u = dist;
                            break;
                        }
                    }
                }

                y_l = y;
                while (line_has_color[y_l][palette[c]] == 0){
                    y_l++;
                    if (y_l >= imgg.rows-1){ y_l = imgg.rows-1; break; }
                }
                max_l = imgg.cols * imgg.rows;
                for (int xx=x; xx >= 0; xx--){
                    if (imgg.ptr<unsigned char>(y_l)[xx] == palette[c]){
                        dist = metric_l(x, y, xx, y_l);
                        if (dist != 0 && dist < max_l){
                            max_l = dist;
                            break;
                        }
                    }
                }
                for (int xx=x; xx < imgg.cols; xx++){
                    if (imgg.ptr<unsigned char>(y_l)[xx] == palette[c]){
                        dist = metric_l(x, y, xx, y_l);
                        if (dist != 0 && dist < max_l){
                            max_l = dist;
                            break;
                        }
                    }
                }
                feature[c] = (max_u < max_l) ? max_u : max_l;
                feature[csize] = imgg.ptr<unsigned char>(y)[x];
            }
            // printf("\n[%2d, %2d] -> ", x, y);
            for (int k=0; k < csize+1; k++){
                //printf("%d, ", feature[k]);
                fprintf(fw, "%d, ", feature[k]);
            }
            fprintf(fw, "\n");
            mat_i++;

        }
    }
    std::cout << "\nVALUES : " << mat_i;
    fclose(fw);
}
