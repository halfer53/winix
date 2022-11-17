#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "../../include_winix/winix/list.h"

#define NUM_COLS                (50)
#define NUM_ROWS                (15)
#define INITIAL_SNAKE_LEN       (3)
#define INITAL_FOOD_NUM         (10)
#define SNAKE_CHAR              ('x')
#define FOOD_CHAR               ('o')
#define GET_SNAKE_HEAD(board)   (list_first_entry(&board->snake, struct point, list))
#define GET_SNAKE_TAIL(board)   (list_last_entry(&board->snake, struct point, list))
#define draw_point(p, c)        draw_coordinate(p->x, p->y, c)
#define clear_point(p)          draw_point(p, ' ')

#define SUCCESS         ( 0)
#define FAIL            (-1)
#define FAIL_LEFT       (-2)
#define FAIL_RIGHT      (-3)
#define FAIL_DOWN       (-4)
#define FAIL_UP         (-5)
#define FAIL_EAT_SELF   (-6)


struct point{
    int x;
    int y;
    struct list_head list;
};

enum direction {left, up, right, down};

struct board_struct{
    struct list_head snake;
    struct list_head foods;
    enum direction dir;
};

struct point* new_point(int x, int y){
    struct point* po = (struct point*)malloc(sizeof(struct point));
    po->x = x;
    po->y = y;
    return po;
}

struct point* new_food(){
    int x = (rand() % (NUM_COLS -2 )) + 1;
    int y = (rand() % (NUM_ROWS -1 )) + 1;
    return new_point(x, y);
}

void disable_cursor(){
    static char str[] = {0x1b, 0x5b ,0x3f ,0x32 ,0x35 ,0x6c, 0};
    printf("%s", str);
}

void int2str(int value, int i, char* output){
    int j;
    while(i){
        j = (value / i) % 10;
        *output++ = '0' + j;
        i /= 10;
    }
}

// byte stream for \e[000;000H
static char draw_pos[] = {0x1b, 0x5b, 0x30, 0x30, 0x30, 0x3b, 0x30, 0x30, 0x30, 0x48, 0};
static char coordinate_format[] = "%s ";

void draw_coordinate(int x, int y, char character){
    char *draw_format = draw_pos;
    int2str(y, 100, draw_format + 2);
    int2str(x, 100, draw_format + 6);
    coordinate_format[2] = character;
    printf(coordinate_format, draw_pos);
    // fprintf(stderr, "Draw %c at x %d y %d | ", character, pos->x, pos->y);
}

void clear_screen(){
    // byte stream for \e[0;0H\e[2J
    static char cls[] = {0x1b, 0x5b, 0x30, 0x3b, 0x30, 0x48, 0x1b, 0x5b, 0x32, 0x4a, 0};
    printf("%s", cls);
}

void print_to_central_screen(int y, char* str){
    int len = strlen(str);
    int x = (NUM_COLS / 2) - (len / 2) - 1;
    draw_coordinate(x, y, ' ');
    printf("%s", str);
}

void print_instruction(){
    int y = NUM_ROWS / 2;
    print_to_central_screen(y++, "Use WASD to move");
    print_to_central_screen(y++, "Press SPACE to start");
    print_to_central_screen(y, "Press q to exit");
}

void print_border(struct board_struct* board){
    char *buffer = malloc(NUM_COLS);
    char *buf = buffer;
    int i, end = NUM_COLS - 2;
    for(i = 0; i < end; i++){
        *buf++ = '-';
    }
    *buf = '\0';
    draw_coordinate(0, 0, '-');
    printf("%s", buffer);
    draw_coordinate(0, NUM_ROWS, '-');
    printf("%s", buffer);
    
    for (i = 1; i < NUM_ROWS; i++){
        draw_coordinate(0, i, '|');
    }
    for (i = 1; i < NUM_ROWS; i++){
        draw_coordinate(NUM_COLS - 1, i, '|');
    }
    free(buffer);
}

void board_init(struct board_struct *board){
    int fd;
    clock_t clo = times(NULL);
    INIT_LIST_HEAD(&board->foods);
    INIT_LIST_HEAD(&board->snake);
    fd = open("/var/log/snake.log", O_RDWR | O_CREAT | O_TRUNC, 0644);
    dup2(fd, STDERR_FILENO);
    close(fd);

    fd = open("/dev/tty1", O_RDWR | O_NONBLOCK);
    dup2(fd, STDIN_FILENO);
    close(fd);

    ioctl(STDIN_FILENO, TIOCDISABLEECHO);
    srand(clo);
    disable_cursor();
}

void init_snake_food(struct board_struct *board){
    int i;
    struct point* food_pos, *snake_pos;
    for(i = 0; i < INITAL_FOOD_NUM; i++){
        food_pos = new_food();
        list_add(&food_pos->list, &board->foods);
        draw_point(food_pos, FOOD_CHAR);
    }
    for(i = 1; i <= INITIAL_SNAKE_LEN; i++){
        snake_pos = new_point(i, 1);
        list_add(&snake_pos->list, &board->snake);
        draw_point(snake_pos, SNAKE_CHAR);
        // fprintf(stderr, "add x %d y %d\n", i, 1);
    }
}

void debug_board(struct board_struct* board){
    struct point* p;
    fprintf(stderr, "points: ");
    list_for_each_entry(struct point, p, &board->snake, list){
        fprintf(stderr, "x %d y %d; ", p->x, p->y);
    }
    fprintf(stderr, "\n");
}


#define INPUT_SIZ   (12)

enum direction get_direction(struct board_struct* board, char c){
    enum direction dir = board->dir;
    //non blocking
    switch (c)
    {
    case 'w':
        if(dir == down)
            return down;
        return up;
    case 'a':
        if(dir == right)
            return right;
        return left; 
    case 's':
        if(dir == up)
            return up;
        return down;
    case 'd':
        if(dir == left)
            return left;
        return right;
    case 'q':
        clear_screen();
        exit(0);

    default:
        break;
    }
    
    return dir;
}

struct point* get_point_at_coordinate(struct list_head* head, int x, int y){
    struct point* p1;
    list_for_each_entry(struct point, p1, head, list){
        if(p1->x == x && p1->y == y){
            return p1;
        }
    }
    return NULL;
}

int _refresh(struct board_struct* board){
    enum direction dir = board->dir;
    struct point* head = GET_SNAKE_HEAD(board), *newpos, *pos;
    int x = head->x, y = head->y;
    // fprintf(stderr, "dir %d | ", dir);
    switch (dir)
    {
    case left:
        if(x == 1)
            return FAIL_LEFT;
        x--;
        break;
    case up:
        if(y == 1)
            return FAIL_UP;
        y--;
        break;
    case right:
        if(x >= NUM_COLS - 2)
            return FAIL_RIGHT;
        x++;
        break;
    case down:
        if(y >= NUM_ROWS - 1)
            return FAIL_DOWN;
        y++;
        break;
    
    default:
        return FAIL;
    }

    pos = get_point_at_coordinate(&board->foods, x, y);
    if(!pos){
        pos = get_point_at_coordinate(&board->snake, x, y);
        if(pos){
            // fprintf(stderr, "eat self x %d y %d\n");
            return FAIL_EAT_SELF;
        }
        pos = GET_SNAKE_TAIL(board);
        clear_point(pos);
    }else{
        // if snake has eaten food, get a new random food
        newpos = new_food();
        list_add(&newpos->list, &board->foods);
        draw_point(newpos, FOOD_CHAR);
    }

    list_del(&pos->list);
    pos->x = x;
    pos->y = y;
    list_add(&pos->list, &board->snake);
    draw_point(pos, SNAKE_CHAR);

    return SUCCESS;
}

int refresh(struct board_struct* board){
    int ret = _refresh(board);
    if(ret){
        print_to_central_screen(NUM_ROWS / 2 - 1, "You lost :(");
        // printf("%d", ret);
    }
    return ret;
}

void draw(struct board_struct *board){
    struct point* p;
    list_for_each_entry(struct point, p, &board->snake, list){
        draw_point(p, SNAKE_CHAR);
    }
    list_for_each_entry(struct point, p, &board->foods, list){
        draw_point(p, FOOD_CHAR);
    }
}

char get_chr(){
    char c;
    int ret = read(STDIN_FILENO, &c, 1 * sizeof(char));
    if(ret > 0)
        return c;
    return 0;
}

void reset_board(struct board_struct* board){
    INIT_LIST_HEAD(&board->snake);
    INIT_LIST_HEAD(&board->foods);
    board->dir = right;
}

int main(int argc, char** argv){
    static char input[INPUT_SIZ];
    static struct board_struct board;
    int ret, i;
    struct board_struct* bp = &board;
    struct timespec ts;
    bool is_snake_alive = true, is_game_running = true, waiting_for_command = true;

    memset(&ts, 0, sizeof(struct timespec));
    board_init(bp);
    clear_screen();
    print_border(bp);

    ts.tv_nsec = 125000000;

    while(is_game_running){

        print_instruction();
        while(waiting_for_command){
            char c = get_chr();
            if(c == 32){
                break;
            }else if (c == 'q'){
                clear_screen();
                return 0;
            }
        }

        reset_board(bp);
        clear_screen();
        print_border(bp);
        init_snake_food(bp);
        is_snake_alive = true;

        while(is_snake_alive){
            char prev = '\0';

            nanosleep(&ts, NULL);
            ret = read(STDIN_FILENO, input, INPUT_SIZ * sizeof(char));
            if (ret == 0){
                if((refresh(bp))){
                    is_snake_alive = false;
                    break;
                }
            }
            for(i = 0; i < ret; i++){
                if (i && input[i] == prev)
                    continue;
                prev = input[i];
                bp->dir = get_direction(bp, input[i]);
                ret = refresh(bp);
                if(ret){
                    is_snake_alive = false;
                    break;
                }
            }
        }
    }
    
    return 0;
}

