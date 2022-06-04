#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <sstream>

struct ppmFile {
    std::string path;
    GLuint width, height, max;
    GLfloat *image = nullptr;
};

ppmFile ppm;

void disp(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(-1, -1);
    glDrawPixels(ppm.width, ppm.height, GL_RGB, GL_FLOAT, ppm.image);
    glFlush();
}

void readPpmFile(const std::string &path) {
    ppm.path = path;
    std::string line;
    std::ifstream ifs(ppm.path);
    if (ifs.fail()) {
        std::cerr << "Failed to open file." << std::endl;
        exit(EXIT_FAILURE);
    }

    // headerのP3は無視する。
    getline(ifs, line);

    // width, heightを取得する。
    getline(ifs, line);
    std::istringstream iss(line);
    iss >> ppm.width >> ppm.height;

    // max値を取得する。
    getline(ifs, line);
    iss = std::istringstream(line);
    iss >> ppm.max;

    // ファイル情報を表示
    std::cout << "path:\t" << ppm.path << std::endl;
    std::cout << "width:\t" << ppm.width << std::endl;
    std::cout << "height:\t" << ppm.height << std::endl;
    std::cout << "max:\t" << ppm.max << std::endl;

    // 画像のピクセルごとの色を格納する配列を初期化する。
    ppm.image = (GLfloat *) malloc(
            sizeof(GLfloat) * ppm.width * ppm.height * 3);
    memset(ppm.image, 0, sizeof(GLfloat) * ppm.width * ppm.height * 3);

    for (int y = 0; y < ppm.height; y++) {
        if (getline(ifs, line)) {
            iss = std::istringstream(line);
            for (int x = 0; x < ppm.width; x++) {
                /**
                 * pixel_idxについて
                 * 今回描画に使用するglDrawPixelsは左下から右上に向かって描画するため、
                 * 画面下に描画される色は配列の初めに保存しなければならない。
                 * よって配列の後ろから保存されるように設定している。
                 */
                const int pixel_idx = ((ppm.height - 1) - y) * ppm.width + x;
                int r, g, b;
                iss >> r >> g >> b;

                ppm.image[pixel_idx * 3] = ((GLfloat) r / ppm.max);
                ppm.image[pixel_idx * 3 + 1] = ((GLfloat) g / ppm.max);
                ppm.image[pixel_idx * 3 + 2] = ((GLfloat) b / ppm.max);
            }
        } else {
            std::cerr << "Failed to load image." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv) {
    // ppmイメージのパスを取得する
    if (argc < 2) {
        std::cerr << "No program parameters provided !" << std::endl;
        exit(EXIT_FAILURE);
    }

    readPpmFile(argv[1]);

    glutInit(&argc, argv);
    glutInitWindowSize(ppm.width, ppm.height);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);

    glutCreateWindow("ppm Viewer");
    glutDisplayFunc(disp);

    glutMainLoop();

    free(ppm.image);
    exit(EXIT_SUCCESS);
}
