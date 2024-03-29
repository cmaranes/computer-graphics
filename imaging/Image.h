//
// Created by Carlos on 22/10/2018.
//

#ifndef COMPUTER_GRAPHICS_IMAGE_H
#define COMPUTER_GRAPHICS_IMAGE_H

// #define MAX 1000

#include "RGB.h"
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

class Image{
public:
    Image(){}
    // We suppose that the file is correct
    Image(string fileName){
        ifstream file;
        file.open(fileName);

        string word;

        // Reading P3
        file >> word;

        // Reading #MAX = ...
        file >> word; // We need parsing
        //m = 65535;
        m = stoi(word.substr(5));

        // Reading filename
        string line;
        file >> word >> word;
        //getline(file, line);

        // Reading x y
        file >> x;
        file >> y;
        image = new RGB[x*y];

        // Reading c, colorResolution
        file >> c;

        // Reading RGB
        float coefficient = 1;
        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                int R, G, B;
                file >> R >> G >> B;
                float vR = (float)R;
                float vG = (float)G;
                float vB = (float)B;
                setPixel(i, j, RGB(vR, vG, vB));
                //image[(i*y)+j] = RGB(vR, vG, vB);
            }
        }

        file.close();
    }

    RGB getPixel(int i, int j){
        //cout<<x*this->y + y<<endl;
        return image[i*this->x + j];
    }

    void setPixel(int i, int j, RGB newPixel){
        image[i*this->x + j] = newPixel;
    }

    int getX(){
        return Image::x;
    }

    int getY(){
        return Image::y;
    }

    void writeImage(string salida){
        ofstream file;
        file.open(salida);
        file << "P3" << endl;
        file << x << " " << y << endl;
        file << "255" << endl;

        float coefficient = 255;
        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                RGB pixel = getPixel(i, j);
                int R, G, B;
                R = pixel.getR() * coefficient;
                G = pixel.getG() * coefficient;
                B = pixel.getB() * coefficient;
                file << R << " " << G << " " << B << "     ";
            }
            file << '\n';
        }

        file.close();
    }

    void equalization(){
        float maxY, minY, X,Y,Z,r,g,b;
        maxY = 0;
        minY = 1000000;
        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                RGB pixel = getPixel(i, j);
                r = pixel.getR();
                g = pixel.getG();
                b = pixel.getB();
                maxY = max(maxY, RGB_Y(r,g,b));
                minY = min(minY, RGB_Y(r,g,b));
            }
        }

        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                RGB pixel = getPixel(i, j);
                r = pixel.getR();
                g = pixel.getG();
                b = pixel.getB();
                X = RGB_X(r,g,b);
                Y = RGB_Y(r,g,b); //* coefficient * m;
                Z = RGB_Z(r,g,b);

                // Paso a xyY
                float x,y;
                x = X / (X+Y+Z);
                y = Y / (X+Y+Z);

                float XNew, YNew, ZNew;
                // equalization
                YNew = Y / (maxY-minY);

                XNew = (YNew * x) / y;
                ZNew = (XNew / x) - XNew - YNew;
                //ZNew = (YNew / y) * ( 1 - x - y );


                pixel.setR(min(XYZ_R(XNew,YNew,ZNew),1));
                pixel.setG(min(XYZ_G(XNew,YNew,ZNew),1));
                pixel.setB(min(XYZ_B(XNew,YNew,ZNew),1));
                setPixel(i, j, pixel);
            }
        }
    }

    //https://stackoverflow.com/questions/16521003/gamma-correction-formula-gamma-or-1-gamma
    // https://docs.opencv.org/3.4/d3/dc1/tutorial_basic_linear_transform.html al final
    void gammaCurve(float factor){
        //equalization();
        float maxY,r,g,b;
        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                RGB pixel = getPixel(i, j);
                r = pixel.getR();
                g = pixel.getG();
                b = pixel.getB();
                maxY = max(maxY, RGB_Y(r,g,b));
            }
        }

        float coefficient = 1/maxY;

        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                float X,Y,Z;
                RGB pixel = getPixel(i, j);
                r = pixel.getR();
                g = pixel.getG();
                b = pixel.getB();
                X = RGB_X(r,g,b);
                Y = RGB_Y(r,g,b); //* coefficient * m;
                Z = RGB_Z(r,g,b);

                float x,y,z;
                x = X / (X+Y+Z);
                y = Y / (X+Y+Z);

                // equalization
                //Y = Y / 65535;
                // gamming
                Y = pow(min(Y * coefficient,1), (1.0 / factor)); //* 65535;
                // Y = pow(Y, 1.0/factor);

                X = (Y / y)*x;
                Z = (Y / y) * (1 - x - y);


                pixel.setR(min(XYZ_R(X,Y,Z), 1));
                pixel.setG(min(XYZ_G(X,Y,Z),1));
                pixel.setB(min(XYZ_B(X,Y,Z),1));
                setPixel(i, j, pixel);
            }
        }
    }

    void gammaCurveClamping(float factor){
        //equalization();
        float maxY,r,g,b;
        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                RGB pixel = getPixel(i, j);
                r = pixel.getR();
                g = pixel.getG();
                b = pixel.getB();
                maxY = max(maxY, RGB_Y(r,g,b));
            }
        }

        float coefficient = 1/maxY;

        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                float X,Y,Z;
                RGB pixel = getPixel(i, j);
                r = pixel.getR();
                g = pixel.getG();
                b = pixel.getB();
                X = RGB_X(r,g,b);
                Y = RGB_Y(r,g,b); //* coefficient * m;
                Z = RGB_Z(r,g,b);

                float x,y,z;
                x = X / (X+Y+Z);
                y = Y / (X+Y+Z);

                // equalization
                //Y = Y / 65535;
                // gamming
                Y = pow(min(Y * coefficient,1), (1.0 / factor)); //* 65535;
                // Y = pow(Y, 1.0/factor);

                //clamping
                Y = min(Y,0.5);

                X = (Y / y)*x;
                Z = (Y / y) * (1 - x - y);


                pixel.setR(min(XYZ_R(X,Y,Z), 1));
                pixel.setG(min(XYZ_G(X,Y,Z),1));
                pixel.setB(min(XYZ_B(X,Y,Z),1));
                setPixel(i, j, pixel);
            }
        }
    }

    void clamping(){
        float X,Y,Z;
        float x,y;
        float r,g,b;
        for (int i = 0; i < this->y; i++){
            for (int j = 0; j < this->x; j++){
                RGB pixel = getPixel(i, j);
                r = pixel.getR(); g = pixel.getG(); b = pixel.getB();
                X = RGB_X(r,g,b);
                Y = RGB_Y(r,g,b);
                Z = RGB_Z(r,g,b);

                float x,y,z;
                x = X / (X+Y+Z);
                y = Y / (X+Y+Z);


                // Clamping
                Y = min(Y, 0.5);

                X = (Y / y)*x;
                Z = (Y / y) * (1 - x - y);

                r = XYZ_R(X,Y,Z); g = XYZ_G(X,Y,Z); b = XYZ_B(X,Y,Z);
                pixel.setR(min(r, 1));
                pixel.setG(min(g,1));
                pixel.setB(min(b,1));
                setPixel(i, j, pixel);
            }
        }
    }

    void equalizeAndClamp(){
        float maxY, X,Y,Z,r,g,b;
        maxY = 0;
        float clamp = 0.3;

        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                RGB pixel = getPixel(i, j);
                r = pixel.getR();
                g = pixel.getG();
                b = pixel.getB();
                maxY = max(maxY, RGB_Y(r,g,b));
            }
        }

        maxY = 0.9 * maxY;

        float coefficient = (1 / maxY);

        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                RGB pixel = getPixel(i, j);
                r = pixel.getR();
                g = pixel.getG();
                b = pixel.getB();
                X = RGB_X(r,g,b);
                Y = RGB_Y(r,g,b); //* coefficient * m;
                Z = RGB_Z(r,g,b);

                float x,y,z;
                x = X / (X+Y+Z);
                y = Y / (X+Y+Z);


                // equalization

                if(Y>maxY){
                    Y=clamp;
                }
                else {
                    Y = min(Y * coefficient,clamp);
                }

                X = (Y / y)*x;
                Z = (Y / y) * (1 - x - y);

                pixel.setR(min(XYZ_R(X,Y,Z),1));
                pixel.setG(min(XYZ_G(X,Y,Z),1));
                pixel.setB(min(XYZ_B(X,Y,Z),1));
                setPixel(i, j, pixel);
            }
        }
    }

    void Reinhard(){
        float maxY, X,Y,Z,r,g,b;
        float sum=0.0;
        maxY = 0;

        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                RGB pixel = getPixel(i, j);
                r = pixel.getR();
                g = pixel.getG();
                b = pixel.getB();
                maxY = max(maxY, RGB_Y(r,g,b));
                sum += log(100+Y);
            }
        }

        float coefficient = 1/maxY;

        float med_Lm = (exp(sum))/(x*y);

        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                RGB pixel = getPixel(i, j);
                r = pixel.getR();
                g = pixel.getG();
                b = pixel.getB();
                X = RGB_X(r,g,b);
                Y = RGB_Y(r,g,b); //* coefficient * m;
                Z = RGB_Z(r,g,b);

                float x,y,z;
                x = X / (X+Y+Z);
                y = Y / (X+Y+Z);

                float Y_2 = (0.18/med_Lm)*(Y);

                Y= (Y_2*(1+(Y_2/(maxY*maxY))))/(1+Y_2);

                X = (Y / y)*x;
                Z = (Y / y) * (1 - x - y);

                pixel.setR(min(XYZ_R(X,Y,Z),1));
                pixel.setG(min(XYZ_G(X,Y,Z),1));
                pixel.setB(min(XYZ_B(X,Y,Z),1));
                setPixel(i, j, pixel);
            }
        }
    }
/*
    void Reinhard(){
        float X,Y,Z,r,g,b;
        for (int i = 0; i < y; i++){
            for (int j = 0; j < x; j++){
                RGB pixel = getPixel(i, j);
                r = pixel.getR();
                g = pixel.getG();
                b = pixel.getB();
                Y = RGB_Y(r,g,b);
                X = RGB_X(r,g,b);
                Z = RGB_Z(r,g,b);

                x = X / (X+Y+Z);
                y = Y / (X+Y+Z);

                Y = Y / (1-Y*R_1(i,j,s_m(i,j)));

                X = (Y / y)*x;
                Z = (Y / y) * (1 - x - y);

                pixel.setR(min(XYZ_R(X,Y,Z),1));
                pixel.setG(min(XYZ_G(X,Y,Z),1));
                pixel.setB(min(XYZ_B(X,Y,Z),1));
                setPixel(i, j, pixel);
            }
        }
    }
*/

private:
    //vector<vector<float>> image;

    // x = anchura
    // y = altura
    // c = colorResolution
    // m = maximumResolution
    int x, y, c;
    float m;

    RGB * image;

    float R_1(int x, int y, float s){
        float alpha = 0.1; //NO FINAL VALUE
        float a = 1/(M_PI*alpha*s);
        float b = (x*x+y*y)/(alpha*alpha*s*s);
        return pow(a,-b);
    }

    float s_m(int x, int y){
        return 0.0;
    }

    float parserMAX(string s){
        const char * c = s.c_str();
        s="";
        for(int i=5;i<s.length();i++){
            s+=c[i];
        }
        return stod(s);
    }

    static float min(float a, float b){
        if(a < b){
            return a;
        }
        else{
            return b;
        }
    }

    static float max(float a, float b){
        if(a > b){
            return a;
        }
        else{
            return b;
        }
    }

    float RGB_X(float r, float g, float b){
        //return 0.4887180 * r + 0.1762044 * g;
        return 0.49*r + 0.31*g + 0.2*b;
    }

    float RGB_Y(float r, float g, float b){
        //return 0.3106803 * r + 0.8129847 * g + 0.0102048 * b;
        return 0.17697*r + 0.8124*g + 0.01063*b;
    }

    float RGB_Z(float r, float g, float b){
        //return 0.2006017 * r + 0.0108109 * g + 0.9897952 * b;
        return  0.01*g  + 0.99*b;
    }

    float XYZ_R(float x, float y, float z){
        //return 2.3706743 * x - 0.5138850 * y + 0.0052982 * z;
        return 2.364614 * x - 0.896541*y - 0.468073*z;
    }

    float XYZ_G(float x, float y, float z){
        //return -0.9000405 * x + 1.4253036 * y - 0.0146949 * z;
        return -0.5138850*x + 1.4253036*y + 0.0885814*z;
    }

    float XYZ_B(float x, float y, float z){
        //return -0.4706338 * x + 0.0885814 * y + 1.0093968 * z;
        return 0.0052982*x - 0.0146949*y  + 1.0093968 *z;
    }

    // https://rosettacode.org/wiki/Gamma_function
    // Existen diferentes implementaciones de la funcion gamma
    //static float gammaFunction(float x){
    //    return sqrt(2*M_PI/x)*pow(x/)
    //}
};

#endif //COMPUTER_GRAPHICS_IMAGE_H
