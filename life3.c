#include <stdio.h>
#include <stdlib.h>
#include <string.h> //memset()関数を使う
#include <unistd.h> // sleep()関数を使う
#include <time.h>
#include <ctype.h> //isdigit()関数を使う

void my_init_cells(const int height, const int width, int cell[height][width], FILE* fp);
void my_print_cells(FILE* fp, int gen, const int height, const int width, int cell[height][width]);
void my_update_cells(const int height, const int width, int cell[height][width]);

int main(int argc, char **argv) {
  FILE *fp = stdout;
  const int height = 40;
  const int width = 70;

  int cell[height][width];
  for(int y = 0 ; y < height ; y++){
    for(int x = 0 ; x < width ; x++){
      cell[y][x] = 0; //とりあえずすべて0にしている
    }
  }

  /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
  if ( argc > 2 ) {
    fprintf(stderr, "usage: %s [filename for init]\n", argv[0]);
    return EXIT_FAILURE;
  }
  else if (argc == 2) {
    FILE *lgfile;
    if ( (lgfile = fopen(argv[1],"r")) != NULL ) {
      my_init_cells(height,width,cell,lgfile); // ファイルによる初期化
    }
    else{
      fprintf(stderr,"cannot open file %s\n",argv[1]);
      return EXIT_FAILURE;
    }
    fclose(lgfile);
  }
  else{
    my_init_cells(height, width, cell, NULL); // デフォルトの初期値を使う
  }

  my_print_cells(fp, 0, height, width, cell); // 表示する
  sleep(1); // 1秒休止
  fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)

  /* 世代を進める*/
  for (int gen = 1 ;; gen++) {
    my_update_cells(height, width, cell); // セルを更新
    my_print_cells(fp, gen, height, width, cell);  // 表示する
    sleep(1); //1秒休止する
    fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)
  }

  return EXIT_SUCCESS;
}

void my_init_cells(const int height, const int width, int cell[height][width], FILE* fp) {
    if (fp == NULL) {
        srand((unsigned int)time(NULL));
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                double r = (double) rand() / RAND_MAX;
                if (r > 0.9) {
                    cell[i][j] = 1;
                }
            }
        }
    }
    else if (getc(fp) == '#') { //Life 1.06形式のとき
        int fileheight[100];
        int filewidth[100];
        char buffer[100];
        int i = 0;
        fgets(buffer, 20, fp);
        while (fscanf(fp, "%d %d\n", filewidth + i, fileheight + i) != EOF) {
            cell[fileheight[i]][filewidth[i]] = 1;
            i++;
        }
    }
    else {  //RLE形式のとき
        int x;
        int y;
        char buffer[100];
        char RLEformat[70]; //RLE形式で表された文字を格納する配列
        int runcount[70]; //数字を格納する配列
        int i = 0; //fscanf用
        int j = 0; //RLEformat用
        int k = 0; //runcount格納用
        int l = 0; //runcount参照用 
        int m = 0; //cellの列数用
        int n = 0; //runcount[k]だけ数字をcellに入れる時のfor文
        int line = 0; //'$'が出るたび行数を1つ上げる
        memset(runcount, 0, 70); //runcountの要素をすべて0にする
        fgets(buffer, 20, fp);
        while (fscanf(fp, "%c", RLEformat + i) != EOF) {
            i++;
        }
        while (RLEformat[j] != '!') {
            if (RLEformat[j] != 'b' && RLEformat[j] !='o' && RLEformat[j] != '$') { //数字のときruncount[]に格納
                if (RLEformat[j+1] != 'b' && RLEformat[j+1] !='o' && RLEformat[j+1] != '$') {
                    int a = RLEformat[j] - '0'; //10の位
                    int b = RLEformat[j+1] - '0'; //1の位
                    runcount[k] = a * 10 + b;
                    k++;
                    j++; //次の数字は参照しないために
                }
                else {
                    int c = RLEformat[j] - '0';
                    runcount[k] = c;
                    k++;
                }
            }
            else {
                if (RLEformat[j] == 'b') { //bのときruncount[l]だけmを増やす
                    if (runcount[l] == 0) {
                        m++;
                    } 
                    else {
                        m += runcount[l];
                        l++;
                    }
                }
                else if (RLEformat[j] == 'o') { //oのとき該当するcellを1に変更
                    if (runcount[l] == 0) {
                        cell[line][m] = 1;
                        m++;
                    } 
                    else {
                        for (int n = 0; n < runcount[l]; n++) {
                            cell[line][m+n] = 1;
                        }
                        m += runcount[l];
                        l++;
                    }
                }
                else if (RLEformat[j] == '$') {
                    line++;
                    m = 0; //列数を初期化
                }
            }
            j++;
        } 
    }
}

void my_print_cells(FILE* fp, int gen, const int height, const int width, int cell[height][width]) {
    printf("generation = %d\n", gen);
    printf("+");
    for (int _ = 0; _ < width; _++) {
        printf("-");
    }
    printf("+\n");
    for (int i = 0; i < height; i++) {
        printf("|");
        for (int j = 0; j < width; j++) {
            if (cell[i][j] == 1) {
                printf("\e[31m#\e[0m");
            }
            else if (cell[i][j] == 0) {
                printf(" ");
            }
        }
        printf("|\n");
    }
    printf("+");
    for (int _ = 0; _ < width; _++) {
        printf("-");
    }
    printf("+\n");
}

int my_count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width]) {
    int count = 0;
    if (w != 0) {
        if (h != 0) {
            if (h == height-1) {
                count += cell[h][w-1] + cell[h-1][w-1];
            }
            else {
                count += cell[h][w-1] + cell[h-1][w-1] + cell[h+1][w-1];
            }
        }
        else {
            count += cell[h][w-1] + cell[h+1][w-1];
        }
    }
    if (w != width-1) {
        if (h != 0) {
            if (h == height-1) {
                count += cell[h][w+1] + cell[h-1][w+1];
            }
            else {
                count += cell[h][w+1] + cell[h-1][w+1] + cell[h+1][w+1];
            }
        }
        else {
            count += cell[h][w+1] + cell[h+1][w+1];
        }
    }
    if (h != 0) {
        count += cell[h-1][w];
    } 
    if (h != height-1) {
        count += cell[h+1][w];
    }
    return count;
}

void my_update_cells(const int height, const int width, int cell[height][width]) {
    int tmp_cell[height][width];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (cell[i][j] == 1) {
                if (my_count_adjacent_cells(i, j, height, width, cell) != 2 && my_count_adjacent_cells(i, j, height, width, cell) != 3) {
                    tmp_cell[i][j] = 0;
                }
                else {
                    tmp_cell[i][j] = 1;
                }
            }
            if (cell[i][j] == 0) {
                if (my_count_adjacent_cells(i, j, height, width, cell) == 3) {
                    tmp_cell[i][j] = 1;
                }
                else {
                    tmp_cell[i][j] = 0;
                }
            }
        }
    }
    for (int k = 0; k < height; k++) {
        for (int l = 0; l < width; l++) {
            cell[k][l] = tmp_cell[k][l];
        }
    }
}