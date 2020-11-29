#include <stdio.h>
#include "get_keyboard.h"
#include <stdlib.h>

int ROWS;
int COLS;
#define MAXLEN 15

#define ROAD  0
#define WALL  1
#define BOX   2
#define TERM  3
#define MOUSE 4

//二维数组 理解成行和列组成(但本质上不是行和列)
//变量在函数外定义  全局变量  在所有的函数内都可以访问和使用

//原始数据
int rootboard[MAXLEN][MAXLEN] = {};

//可供操作
int board[MAXLEN][MAXLEN] = {};

//记录小老鼠的位置 
int mx = 0,my = 0;
//记录箱子的个数
int boxnum = 0;


//记录当前箱子到达终点的个数
int boxintermnum = 0;

//用于记录回退的数据
int direction; //方向
//是否可以回退
int isback = 0;//0不可以回退  1可以回退
int boxismove = 0;//0 箱子没有移动  1箱子移动



void show(){
	int row,col;
	boxintermnum = 0;
	for(row=0;row<ROWS;row++){
		//board[row]
		for(col=0;col<COLS;col++){
			//printf("%d ",board[row][col]);
			switch(board[row][col]){
				case ROAD:
					printf(" ");break;
				case WALL:
					printf("#");break;
				case BOX:
					printf("@");break;
				case TERM:
					printf("O");break;
				case MOUSE:
					printf("&");break;
				case MOUSE+TERM:
					printf("&");break;
				case BOX+TERM:
					boxintermnum = boxintermnum + 1;
					printf("@");break;			
			}
		}
		printf("\n");
	}
}



//复位
void reset(){
	int row,col;
	boxnum = 0;
	for(row=0;row<ROWS;row++){
		for(col=0;col<COLS;col++){
			board[row][col] = rootboard[row][col];
			if(board[row][col] == MOUSE){
				mx = row;
				my = col;
			}else if(board[row][col] == BOX ||board[row][col]==BOX+TERM){//该位置是箱子
				boxnum = boxnum+1;
			}
		}
	}
}

void loadboard(int cnt){
	//加载第一关
	char filename[40] = {};
	sprintf(filename,"%d.txt",cnt);
	FILE *fp = fopen(filename,"r");
	if(fp == NULL){
		printf("加载失败！\n");
		exit(-1);
	}
	fscanf(fp,"%d %d",&ROWS,&COLS);
	int row,col;
	for(row=0;row<ROWS;row++){
		for(col=0;col<COLS;col++){
			fscanf(fp,"%d",&rootboard[row][col]);
		}
	}
	fclose(fp);
}

//"加载地图"   找出小老鼠的位置  找出箱子的个数
void init(int cnt){
	loadboard(cnt);
	reset();
}

//1 只有老鼠移动  2 箱子移动   0没有移动
int move(int stepx,int stepy){
	//小老鼠前一个位置
	int nx = mx+stepx;
	int ny = my+stepy;
	//下一个位置是  路  或者  终点
	if(board[nx][ny] == ROAD || board[nx][ny] == TERM){
		//恢复小老鼠原来位置上东西
		board[mx][my] = board[mx][my]-MOUSE;
		//小老鼠到新的位置 老鼠站在新的东西上面
		board[nx][ny] = MOUSE+board[nx][ny];
		mx = nx;
		my = ny;
		return 1;
	}else if(board[nx][ny] == BOX ||board[nx][ny] == BOX+TERM){//前面是箱子
		//小老鼠前两个位置  箱子前面
		int nnx = mx+2*stepx;
		int nny = my+2*stepy;   
		if(board[nnx][nny] == ROAD || board[nnx][nny] == TERM){
			//箱子先动 然后 老鼠再动
			board[nnx][nny] = board[nnx][nny]+BOX;
			board[nx][ny] = board[nx][ny] - BOX;
			board[mx][my] = board[mx][my] - MOUSE;
			board[nx][ny] = board[nx][ny] + MOUSE;
			mx = nx;
			my = ny;
			return 2;
		}
	}
	return 0;
}

void moveback(int stepx,int stepy){
	int nx = mx+stepx;
	int ny = my+stepy;
	board[nx][ny] = board[nx][ny]+MOUSE;
	board[mx][my] = board[mx][my]-MOUSE;
	if(boxismove==1){
		board[mx][my] = board[mx][my] + BOX;
		board[mx-stepx][my-stepy] = board[mx-stepx][my-stepy] - BOX;
		boxismove = 0;
	}
	mx = nx;
	my = ny;
}
//回退  之前的状态 地图 小老鼠位置 需要保存 
//方向键 箱子有没有动   栈  
void back(){
	if(isback == 1){//可以回退
		switch(direction){
			case KEY_UP:
				moveback(1,0);break;
			case KEY_DOWN:
				moveback(-1,0);break;
			case KEY_LEFT:
				moveback(0,1);break;
			case KEY_RIGHT:
				moveback(0,-1);break;
		}
		isback = 0;
	}
}


void run(){
	int cnt = 0;
	for(cnt=1;cnt<=5;cnt++){
		init(cnt);//初始化  "加载地图" 找出小老鼠的位置  找出箱子的个数
		printf("(%d,%d)\n",mx,my);
		printf("boxnum:%d\n",boxnum);
	
		for(;;){
			system("clear");
			show();//调用函数时  直接函数名(参数)
			if(boxintermnum == boxnum){
				printf("Congraltion!\n");
				printf("下一关 or 退出？");
				int choice = get_keyboard();
				if(choice==KEY_q){
					exit(0);
				}
				break;//跳出循环
			}
			int input = get_keyboard();
			int ret = 0;
			switch(input){
				case KEY_UP:
					ret = move(-1,0);break;
				case KEY_DOWN:
					ret = move(1,0);break;
				case KEY_LEFT:
					ret = move(0,-1);break;
				case KEY_RIGHT:
					ret = move(0,1);break;
				case KEY_r://复位
					reset();break;
				case KEY_q://退出
					exit(0);//退出整个程序
				case KEY_BACKSPACE://Backspace
					back();
					break;
			}
			if(ret != 0){//移动了
				isback = 1;
				if(ret == 2){
					boxismove = 1;
				}
				direction = input;
			}
		}
	}
}

int main(){
	run();
	return 0;
}

