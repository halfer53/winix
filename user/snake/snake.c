#include <stdio.h>
#include <winix/list.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define NUM_COLS  (100)
#define NUM_ROWS  (31)
#define INITIAL_SNAKE_LEN   (3)
#define INITAL_FOOD_NUM (5)
#define SNAKE_CHAR  ('x')
#define FOOD_CHAR   ('o')
#define GET_SNAKE_HEAD(board)   (list_first_entry(&board->snake, struct point, list))

#define SUCCESS (0)
#define FAIL    (-1)
#define FAIL_LEFT   (-2)
#define FAIL_RIGHT   (-3)
#define FAIL_DOWN   (-4)
#define FAIL_UP   (-5)
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
    int ofd;
};

struct point* new_point(int x, int y){
    struct point* po = (struct point*)malloc(sizeof(struct point));
    po->x = x;
    po->y = y;
    return po;
}

struct point* new_food(){
    struct point* p;
    int x = rand() % NUM_COLS;
    int y = rand() % NUM_ROWS;
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

void draw_point(struct point* pos, char character){
    static char draw_pos[] = {0x1b, 0x5b, 0x30, 0x30, 0x30, 0x3b, 0x30, 0x30, 0x30, 0x48, 0};
    static char format[] = "%s ";
    char buf[2];
    char *draw_format = draw_pos;
    int2str(pos->y, 100, draw_format + 2);
    int2str(pos->x, 100, draw_format + 6);
    format[2] = character;
    printf(format, draw_pos);
    buf[0] = character;
    buf[1] = '\0';
    // fprintf(stderr, "Draw %s at x %d y %d | ", buf, pos->x, pos->y);
}

void clear_point(struct point* p){
    draw_point(p, ' ');
}

void clear_screen(){
    // byte stream for \e[2J
    // static char cls[] = {0x1b, 0x5b, 0x31, 0x3b, 0x31, 0x48, 0x1b, 0x5b, 0x32, 0x4a, 0};
    static char cls[] = {0x1b, 0x5b, 0x32, 0x4a, 0};

    printf("%s", cls);
}

void board_init(struct board_struct *board){
    int i, x, y, fd, fd2;
    struct point* food_pos, *snake_pos;
    INIT_LIST_HEAD(&board->foods);
    INIT_LIST_HEAD(&board->snake);
    fd = open("/dev/tty2", O_RDWR);
    dup2(fd, STDERR_FILENO);
    fd2 = open("/dev/tty1", O_RDWR | O_NONBLOCK);
    board->ofd = fd2;
    ioctl(fd2, TIOCDISABLEECHO);
    dup2(fd2, STDIN_FILENO);
    disable_cursor();
    clear_screen();

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

enum direction get_direction(struct board_struct* board, enum direction dir){
    char input[INPUT_SIZ];
    enum direction newdir;
    int i;
    //non blocking
    int ret = read(board->ofd, input, INPUT_SIZ);
    if(ret > 0){
        char c = input[ret - 1];
        // fprintf(stderr, "Direction %d \n",ret, c);
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

        default:
            break;
        }
    }
    return dir;
}

struct point* get_snake_at_coordinate(struct board_struct* board, int x, int y){
    struct point* p1, *p2;
    list_for_each_entry_safe(struct point, p1, p2, &board->snake, list){
        if(p1->x == x && p1->y == y){
            return p1;
        }
    }
    return NULL;
}

struct point* get_food_at_coordinate(struct board_struct* board, int x, int y){
    struct point* p1, *p2;
    list_for_each_entry_safe(struct point, p1, p2, &board->foods, list){
        if(p1->x == x && p1->y == y){
            return p1;
        }
    }
    return NULL;
}

int refresh(struct board_struct* board, enum direction dir){
    struct point* head = GET_SNAKE_HEAD(board), *newpos, *pos;
    int x = head->x, y = head->y;
    // fprintf(stderr, "dir %d | ", dir);
    switch (dir)
    {
    case left:
        if(x == 0)
            return FAIL_LEFT;
        x--;
        break;
    case up:
        if(y == 0)
            return FAIL_UP;
        y--;
        break;
    case right:
        if(x >= NUM_COLS)
            return FAIL_RIGHT;
        x++;
        break;
    case down:
        if(y >= NUM_ROWS)
            return FAIL_DOWN;
        y++;
        break;
    
    
    default:
        return FAIL;
    }

    pos = get_food_at_coordinate(board, x, y);
    if(!pos){
        pos = get_snake_at_coordinate(board, x, y);
        if(pos){
            return FAIL_EAT_SELF;
        }
        pos = list_last_entry(&board->snake, struct point, list);
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


// \e[1;1H


void draw(struct board_struct *board){
    struct point* p;
    list_for_each_entry(struct point, p, &board->snake, list){
        draw_point(p, 'x');
    }
    list_for_each_entry(struct point, p, &board->foods, list){
        draw_point(p, 'o');
    }
}

int main(int argc, char** argv){
    int ret;
    struct board_struct board;
    struct board_struct* bp = &board;
    enum direction dir = right;
    board_init(bp);
    while(1){
        int tmp = 20000;
        dir = get_direction(bp, dir);
        ret = refresh(bp, dir);
        if(ret){
            clear_screen();
            printf("\n You lost :( \n");
            break;
        }
        while(tmp--);
    }
    return 0;
}

