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
    int speed;
    int score;
    int scoreSpeed;
    int powerup;
    int powerupColor;
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




Mix_Music *titleMusic;
Mix_Music *gameplayMusic;
Mix_Chunk *eatSound;
Mix_Chunk *deathSound;
Mix_Chunk *menuSound;
Mix_Chunk *powerupSound;

int lastMove = 0;
int kobraSpeed;

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


    titleMusic = Mix_LoadMUS("audio/bgmusic.wav");
    gameplayMusic = Mix_LoadMUS("audio/gameplay.wav");
    eatSound = Mix_LoadWAV("audio/eatsound.wav");
    deathSound = Mix_LoadWAV("audio/deathsound.wav");
    menuSound = Mix_LoadWAV("audio/option.wav");
    powerupSound = Mix_LoadWAV("audio/powerup.wav");


    while(true){
        initscr();
        cbreak();
        noecho();
        curs_set(0);

        int gamePoint = gameMenu();
        if(gamePoint == 1){
            srand(time(NULL));

            Mix_PlayMusic(gameplayMusic, -1);

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
            kobra.score = 0;
            kobra.speed = 200;
            kobra.scoreSpeed = 0;
            kobra.powerup = 0;
            kobra.powerupColor = 0;

            kobra.bodyX = malloc(sizeof(int) * kobra.length);
            kobra.bodyY = malloc(sizeof(int) * kobra.length);

            Food food;

            food.Y = row/2;
            food.X = col/2 + 8;
            mvprintw(food.Y, food.X, "*");

            //attron(COLOR_PAIR(1));
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
            //attroff(COLOR_PAIR(1));


            //Actual game
            while(isAlive){
                    mvprintw(row + 2, col/2 - 4, "SCORE: %d", kobra.score);
                    if(!kobra.powerup){
                        //attron(COLOR_PAIR(1));
                        moveKobra(&kobra, &direcao);
                        //attroff(COLOR_PAIR(1));
                    }
                    else{
                        //attron(COLOR_PAIR(2));
                        moveKobra(&kobra, &direcao);
                        if(kobra.powerupColor == kobra.length / 3){
                            kobra.powerup = 0;
                            kobra.powerupColor = 0;
                        }
                    }

                if(kobra.bodyX[0] == food.X && kobra.bodyY[0] == food.Y){
                    kobra.length += 1;
                    kobra.score += 10;
                    kobra.scoreSpeed += 10;
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

                        mvprintw(row + 4, col/2 - 10, "[ G A M E  O V E R ]");
                        mvprintw(row + 6, col/2 - 12, "Press ANY KEY to continue...");
                        Mix_PauseMusic();
                        Mix_PlayChannel(-1, deathSound, 0);
                        char userInput = getch();

                        clear();
                        endwin();



                    }

                    for(int i = 1; i < kobra.length; i++){
                        if(kobra.bodyX[0] == kobra.bodyX[i] && kobra.bodyY[0] == kobra.bodyY[i]){
                            mvprintw(kobra.bodyY[0], kobra.bodyX[0], "X");
                            nodelay(stdscr, FALSE);
                            isAlive = 0;

                            mvprintw(row + 4, col/2 - 10, "[ G A M E  O V E R ]");
                            mvprintw(row + 6, col/2 - 12, "Press ANY KEY to continue...");
                            Mix_PauseMusic();
                            Mix_PlayChannel(-1, deathSound, 0);

                            char userInput = getch();
                            clear();
                            endwin();

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
        mvprintw(row + 8, i, "-");
    }

    //Write vertical border of map
    for(int i = 1; i < row + 8; i++){
        mvprintw(i, 0, "|");
        mvprintw(i, col - 1, "|");
    }


}

void moveKobra(Kobra *kobra, int *direction){

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);


    attron(COLOR_PAIR(1));

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
    attroff(COLOR_PAIR(1));

    if(kobra->powerup && kobra->powerupColor < kobra->length){
        attron(COLOR_PAIR(2));
        for(int i = 0; i == 0 || i < kobra->length; i++){
            if(i == 0){
                mvprintw(kobra->bodyY[i], kobra->bodyX[i], "@");
            }
            else{
                mvprintw(kobra->bodyY[i], kobra->bodyX[i], "o");
            }
        }
        kobra->powerupColor+=1;
        attroff(COLOR_PAIR(2));
    }


    flushinp();



    if(kobra->scoreSpeed != 0 && kobra->scoreSpeed % 100 == 0){
        kobra->powerup = 1;
        kobra->speed -= 20;
        kobra->scoreSpeed = 0;
        Mix_PlayChannel(-1, powerupSound, 0);
    }
    //mvprintw(row/2, col/2, "%d", kobra->score);
    napms(kobra->speed);
}

void spawnFood(Food *food, int col, int row){
        food->X = ((rand() % (col - 6) + 2)/2) * 2;
        food->Y = ((rand() % (row - 5)) + 3);
        mvprintw(food->Y, food->X, "*");
}

int gameMenu(){
    if(!Mix_PlayingMusic() || Mix_PausedMusic()){
        Mix_PlayMusic(titleMusic, -1);
    }


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

        //KOBRA TITLE
        mvprintw(menuRow/2 - 10, menuCol/2 - 20,  " _  __   ___    ____    ____       _");
        mvprintw(menuRow/2 - 9, menuCol/2 - 20,  "| |/ /  / _ \\  | __ )  |  _ \\     / \\");
        mvprintw(menuRow/2 - 8, menuCol/2 - 20,  "| ' /  | | | | |  _ \\  | |_) |   / _ \\  ");
        mvprintw(menuRow/2 - 7, menuCol/2 - 20,  "| . \\  | |_| | | |_) | |  _ <   / ___ \\ ");
        mvprintw(menuRow/2 - 6, menuCol/2 - 20,  "|_|\\_\\  \\___/  |____/  |_| \\_\\ /_/   \\_\\");






        char options[3][13] = {"Play", "Instructions", "Quit"};
        for(int i = 0; i < 3; i++){
            mvprintw(menuRow/2 + ((i + 1) * 2), menuCol/2 - 6, "%s", options[i]);
        }

        if(optionPicked == 1 || optionPicked == 4){
            mvprintw(menuRow/2 + 6, menuCol/2 - 8, " ");
            mvprintw((menuRow/2 + 2), menuCol/2 - 8, ">");
            optionPicked = 1;
        }


        int userInput = getch();

        //Navegating through menu options
        if(userInput == KEY_DOWN && optionPicked != 3){
            optionPicked+=1;
            mvprintw(optionPosition, menuCol/2 - 8, " ");
            optionPosition+=2;
            mvprintw(optionPosition, menuCol/2 - 8, ">");
            Mix_PlayChannel(-1, menuSound, 0);
        }
        if(userInput == KEY_UP && optionPicked != 1){
            optionPicked-=1;
            mvprintw(optionPosition, menuCol/2 - 8, " ");
            optionPosition-=2;
            mvprintw(optionPosition, menuCol/2 - 8, ">");
            Mix_PlayChannel(-1, menuSound, 0);
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

    char instructions[4][60] = {"Use ARROWS to control Kobra", "You must feed Kobra by eating the food (*)", "If Kobra hits any place with its head, its over!", "Press ANY KEY to continue..."};
    mvprintw(optionRow/2 - 12, optionCol/2 - 6, "INSTRUCTIONS");
    for(int i = 0; i < 4; i++){
        mvprintw(optionRow/2 + (i * 1), optionCol/2 - 20, "%s", instructions[i]);

    }

    int userInput = getch();
    clear();

}

void gameSound(){

}



