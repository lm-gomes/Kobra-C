#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <ncurses.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>




//Struct Kobra character
typedef struct{
    int *bodyX;
    int *bodyY;
    int length;
    int score;
}Kobra;

typedef struct{
    int X,Y;

}Food;

//Initializing functions
int gameMenu();
void map(int row, int col);
void moveKobra(Kobra *kobra, int *direction);
void spawnFood(Food *food, int col, int row);
void optionsMenu();






int lastMove = 0;

int main()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    printf("Erro ao iniciar SDL: %s\n", SDL_GetError());
    return 1;
}

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erro ao iniciar SDL_mixer: %s\n", Mix_GetError());
        return 1;
    }

    Mix_Chunk *eatSound = Mix_LoadWAV("audio/eatsound.wav");
    Mix_Music *bgMusic = Mix_LoadMUS("audio/bgmusic.wav");

    while(true){
        initscr();
        cbreak();
        noecho();
        curs_set(0);

        Mix_PlayMusic(bgMusic, -1);
        int gamePoint = gameMenu();
        if(gamePoint == 1){
            srand(time(NULL));


            //Game varia1bles
            int row = 30, col = 68;
            int direcao = KEY_LEFT;
            int isAlive = 1;

            //Setting first configurations to start the game
            initscr();
            //getmaxyx(stdscr,row,col);
            cbreak();
            noecho();
            curs_set(0);
            map(row, col);
            nodelay(stdscr, TRUE);
            keypad(stdscr, TRUE);

            //Kobra initializer
            Kobra kobra;
            kobra.length = 3;
            kobra.bodyX = malloc(sizeof(int) * kobra.length);
            kobra.bodyY = malloc(sizeof(int) * kobra.length);

            Food food;

            food.Y = row/2;
            food.X = col/2 + 8;
            mvprintw(food.Y, food.X, "*");

            for(int i = 0; i < 3; i++){
                kobra.bodyY[i] = row/2;
                kobra.bodyX[i] = col/2 + i * 2;
                if(i == 0){
                    mvprintw(kobra.bodyY[i], kobra.bodyX[i], "@");
                }
                else{
                    mvprintw(kobra.bodyY[i], kobra.bodyX[i], "o");
                }
            }


            //Actual game
            while(isAlive){
                    moveKobra(&kobra, &direcao);

                if(kobra.bodyX[0] == food.X && kobra.bodyY[0] == food.Y){
                    kobra.length += 1;
                    kobra.score += 10;
                    kobra.bodyX = realloc(kobra.bodyX, sizeof(int) * kobra.length);
                    kobra.bodyY = realloc(kobra.bodyY, sizeof(int) * kobra.length);
                    spawnFood(&food, col, row);
                    Mix_PlayChannel(-1, eatSound, 0);

                }
                else{
                    //Detecting collision with walls
                    if((kobra.bodyX[0] == 0 || kobra.bodyY[0] == 0) || (kobra.bodyX[0] == col - 2) || kobra.bodyY[0] == row - 2){
                        mvprintw(kobra.bodyY[0], kobra.bodyX[0], "X");
                        nodelay(stdscr, FALSE);
                        isAlive = 0;

                        mvprintw(row/2, col/2, "[GAME OVER]\nPress ENTER to exit.");
                        char userInput = getch();
                        if(userInput == '\n'){
                            clear();
                            endwin();

                        }
                    }

                    for(int i = 1; i < kobra.length; i++){
                        if(kobra.bodyX[0] == kobra.bodyX[i] && kobra.bodyY[0] == kobra.bodyY[i]){
                            mvprintw(kobra.bodyY[0], kobra.bodyX[0], "X");
                            nodelay(stdscr, FALSE);
                            isAlive = 0;

                            mvprintw(row/2, col/2, "[GAME OVER]\nPress ENTER to exit.");
                            char userInput = getch();
                            if(userInput == '\n'){
                                clear();
                                endwin();

                            }
                        }

                    }


                    //If food spawns inside kobra
                    for(int i = 0; i < kobra.length; i++){
                        if(kobra.bodyX[i] == food.X && kobra.bodyY[i] == food.Y){
                            spawnFood(&food, col, row);
                        }
                    }

                }
                refresh();
            }
        }
        else if(gamePoint == 2){
                optionsMenu();
        }
        else{
            endwin();
            return 0;
        }
    }

    endwin();
    return 0;
}

void map(int row, int col){
    //Write horizontal border of map
    for(int i = 1; i < col - 1; i+=2){
        mvprintw(0, i, "-");
        mvprintw(row - 1, i, "-");
    }

    //Write vertical border of map
    for(int i = 1; i < row - 1; i++){
        mvprintw(i, 0, "|");
        mvprintw(i, col -1, "|");
    }
}

void moveKobra(Kobra *kobra, int *direction){
    int move = getch();

    if(move != ERR){
        if((*direction == KEY_UP && move != KEY_DOWN) || (*direction == KEY_DOWN && move != KEY_UP)) {
            *direction = move;
        }
        else if((*direction == KEY_RIGHT && move != KEY_LEFT) || (*direction == KEY_LEFT && move != KEY_RIGHT)){
            *direction = move;
        }

    }

    if(*direction == KEY_LEFT){
        lastMove = KEY_LEFT;

        mvprintw(kobra->bodyY[kobra->length - 1], kobra->bodyX[kobra->length - 1], " ");

        for(int i = kobra->length - 1; i > 0; i--){
            kobra->bodyX[i] = kobra->bodyX[i - 1];
            kobra->bodyY[i] = kobra->bodyY[i - 1];
        }
        kobra->bodyX[0] -= 2;

        for(int i = 0; i < kobra->length; i++){
            if(i == 0){
                mvprintw(kobra->bodyY[i], kobra->bodyX[i], "@");
            }
            else{
                mvprintw(kobra->bodyY[i], kobra->bodyX[i], "o");
            }
        }

    }

    if(*direction == KEY_RIGHT){
        lastMove = KEY_RIGHT;

        mvprintw(kobra->bodyY[kobra->length - 1], kobra->bodyX[kobra->length - 1], " ");

        for(int i = kobra->length - 1; i > 0; i--){
            kobra->bodyX[i] = kobra->bodyX[i - 1];
            kobra->bodyY[i] = kobra->bodyY[i - 1];
        }
        kobra->bodyX[0] += 2;

        for(int i = 0; i < kobra->length; i++){
            if(i == 0){
                mvprintw(kobra->bodyY[i], kobra->bodyX[i], "@");
            }
            else{
                mvprintw(kobra->bodyY[i], kobra->bodyX[i], "o");
            }
        }

    }

    if(*direction == KEY_UP){
        lastMove = KEY_UP;

        mvprintw(kobra->bodyY[kobra->length - 1], kobra->bodyX[kobra->length - 1], " ");

        for(int i = kobra->length - 1; i > 0; i--){
            kobra->bodyX[i] = kobra->bodyX[i - 1];
            kobra->bodyY[i] = kobra->bodyY[i - 1];
        }
        kobra->bodyY[0] -= 1;

        for(int i = 0; i < kobra->length; i++){
            if(i == 0){
                mvprintw(kobra->bodyY[i], kobra->bodyX[i], "@");
            }
            else{
                mvprintw(kobra->bodyY[i], kobra->bodyX[i], "o");
            }
        }

    }

    if(*direction == KEY_DOWN){
        lastMove = KEY_DOWN;
        mvprintw(kobra->bodyY[kobra->length - 1], kobra->bodyX[kobra->length - 1], " ");

        for(int i = kobra->length - 1; i > 0; i--){
            kobra->bodyX[i] = kobra->bodyX[i - 1];
            kobra->bodyY[i] = kobra->bodyY[i - 1];
        }
        kobra->bodyY[0] += 1;

        for(int i = 0; i < kobra->length; i++){
            if(i == 0){
                mvprintw(kobra->bodyY[i], kobra->bodyX[i], "@");
            }
            else{
                mvprintw(kobra->bodyY[i], kobra->bodyX[i], "o");
            }
        }
    }



    flushinp();
    napms(200);

}

void spawnFood(Food *food, int col, int row){
        food->X = ((rand() % (col - 6) + 2)/2) * 2;
        food->Y = ((rand() % (row - 5)) + 3);
        mvprintw(food->Y, food->X, "*");
}

int gameMenu(){

    int menuRow = 40, menuCol = 68;
    int optionPicked = 1;
    int optionPosition = menuRow/2 + 2;
    keypad(stdscr, TRUE);

    while(true){

        //Write horizontal border of gametitle
        for(int i = 0; i < menuCol; i+=2){
            mvprintw(0, i, "-");
            mvprintw(menuRow - 1, i, "-");
        }

        //Write vertical border of gametitle
        for(int i = 0; i < menuRow; i++){
            mvprintw(i, 0, "|");
            mvprintw(i, menuCol -1, "|");
        }

        mvprintw(menuRow/2 - 2, menuCol/2 - 4, "K O B R A");

        char options[3][13] = {"Play", "Instructions", "Exit"};
        for(int i = 0; i < 3; i++){
            mvprintw(menuRow/2 + ((i + 1) * 2), menuCol/2 - 4, "%s", options[i]);
        }

        if(optionPicked == 1 || optionPicked == 4){
            mvprintw(menuRow/2 + 6, menuCol/2 - 6, " ");
            mvprintw((menuRow/2 + 2), menuCol/2 - 6, ">");
            optionPicked = 1;
        }


        int userInput = getch();

        //Navegating through menu options
        if(userInput == KEY_DOWN && optionPicked != 3){
            optionPicked+=1;
            mvprintw(optionPosition, menuCol/2 - 6, " ");
            optionPosition+=2;
            mvprintw(optionPosition, menuCol/2 - 6, ">");
        }
        if(userInput == KEY_UP && optionPicked != 1){
            optionPicked-=1;
            mvprintw(optionPosition, menuCol/2 - 6, " ");
            optionPosition-=2;
            mvprintw(optionPosition, menuCol/2 - 6, ">");
        }

        //When an option is selected, returns an integer

        if(userInput == 10){
            clear();
            return optionPicked;
        }
    }
}

void optionsMenu(){
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    int optionRow = 40, optionCol = 68;

    //Write horizontal border of gametitle
        for(int i = 0; i < optionCol; i+=2){
            mvprintw(0, i, "-");
            mvprintw(optionRow - 1, i, "-");
        }

        //Write vertical border of gametitle
        for(int i = 0; i < optionRow; i++){
            mvprintw(i, 0, "|");
            mvprintw(i, optionCol -1, "|");
        }

    char instructions[4][60] = {"Use ARROWS to control Kobra", "You must feed Kobra by eating the food (*)", "If Kobra hits any place with its head, its over!", "Press [ENTER] to continue..."};
    mvprintw(optionRow/2 - 12, optionCol/2 - 6, "INSTRUCTIONS");
    for(int i = 0; i < 4; i++){
        mvprintw(optionRow/2 + (i * 1), optionCol/2 - 20, "%s", instructions[i]);

    }

    int userInput = getch();
    if(userInput == 10){
        clear();
    }
}

void gameSound(){

}



