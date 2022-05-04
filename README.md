# lifegame
**ソフトウェアⅡ第1回**

## mylife1.c

my_init_cells, my_print_cells, my_update_cells, my_count_adjacent_cells(my_update_cellsに必要)を実装した上でsrand()関数を用いて引数がないときの初期配置を乱数で決定できるようにしました。  
srandの種を時間によって変化するようにしたので実行するたびに初期位置が変わります。  
生存セルの割合が10％程度という制約は 0 <= rand() / RAND_MAX <= 1 という性質を用いて定義しました。

## mylife2.c

sprintf()関数を用いてgenに依存したファイル名に100世代ごとの状態を#Life 1.06形式で出力できるようにしました。  
my_print_cellsはsleep()関数を削除したことでみやすさがかなり悪くなってしまっています（目視で世代ごとの変遷を認識するのは困難です）が課題2はそこが主目的ではないので許容しました。

## mylife3.c

my_init_cellsを更新してRLE形式のときも読み込めるようにしました。  
今回は入力がx <= width, y <= height を満たすことを前提として進めました。  
RLE形式ファイルの読み込み方としては2行目を1文字ずつ見ていき(RLEformat[]に格納されている)b,o,$でないとき数字用の配列runcount[]に追加、b,oのときruncount[]を参照して必要な変更をする、$のとき行数を1増やすといった方式をとりました。  
また妥協案にはなりますが今回 width < 100 であることから連続でbやoを並べる数は2桁までであることを前提としました。

## mylife4.c（発展課題）

mylife1をベースとしてヤギと草のライフゲームを作成しました。 ルールは以下を参照のこと。
* ヤギは生きているとき周りにある草が3つ以下であれば死亡 
* ヤギが死んでいるとき周りに2匹以上ヤギが存在し、かつ周りに少なくとも1つ草があるときに誕生 
* 草は生きているとき周りに2匹以上ヤギがいれば死亡 
* 草が死んでいるとき周りに3つ以上草があれば誕生 
* 草とヤギの誕生条件をどちらも満たすときはヤギの誕生を優先 
* 初期配置は約10％がヤギ、それ以外は全て草となるようにした（mylife1のsrand()を引き続き利用した） 
