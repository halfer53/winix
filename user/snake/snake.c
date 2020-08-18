#include <stdio.h>
#include <winix/list.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#define NUM_COLS  (100)
#define NUM_ROWS  (32)
#define INITIAL_SNAKE_LEN   (1)
#define INITAL_FOOD_NUM (5)
#define SNAKE_HEAD(board)   (list_first_entry(&board->snake, struct point, list))

#define FAIL    (-1)
#define SUCCESS (0)

struct point{
    int x;
    int y;
    struct list_head list;
};

enum direction {left, up, right, down};

struct board_struct{
    struct point snake_head;
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

void delete_point(struct point* p){
    free(p);
}

void board_init(struct board_struct *board){
    static char disable_cursor[] = { 0x1b,0x5B ,0x3F, 0x32 ,0x35 ,0x6C, 0 };
    int i, x, y, fd, fd2;
    struct point* snake_pos, *food_pos;
    printf("%s", disable_cursor);
    srand(times(NULL));
    INIT_LIST_HEAD(&board->snake);
    INIT_LIST_HEAD(&board->foods);
    
    for(i = 0; i < INITAL_FOOD_NUM; i++){
        x = rand() % NUM_COLS;
        y = rand() % NUM_ROWS;
        food_pos = new_point(x, y);
        list_add(&food_pos->list, &board->foods);
    }
    for(i = 1; i <= INITIAL_SNAKE_LEN; i++){
        snake_pos = new_point(1, i);
        list_add(&snake_pos->list, &board->snake);
    }
    fd = open("/dev/tty2", O_RDWR);
    dup2(fd, STDERR_FILENO);
    fd2 = open("/dev/tty1", O_RDWR | O_NONBLOCK);
    // ioctl(fd2, TIOCDISABLEECHO);
    dup2(fd2, STDIN_FILENO);
    // printf("fd %d\n", fd);
    // exit(0);
}

void clear_screen(){
    // byte stream for \e[2J
    static char cls[] = { 0x1b, 0x5b, 0x32, 0x4a, 0};
    printf("%s", cls);
}

void debug_board(struct board_struct* board){
    struct point* p;
    fprintf(stderr, "points: ");
    list_for_each_entry(struct point, p, &board->snake, list){
        fprintf(stderr, "x %d y %d; ", p->x, p->y);
    }
    fprintf(stderr, "\n");
}

enum direction get_direction(enum direction dir){
    char input;
    enum direction newdir;
    //non blocking
    int ret = read(STDIN_FILENO, &input, 1);
    if(ret == 1){
        fprintf(stderr, "ret %d ", input);
        switch (input)
        {
        case 'w':
            // if(dir == down)
            //     return down;
            return up;
        case 'a':
            // if(dir == right)
            //     return right;
            return left;
        case 's':
            // if(dir == up)
            //     return up;
            return down;
        case 'd':
            // if(dir == left)
            //     return left;
            return right;

        default:
            return down;
        }
    }
    return dir;
}

int refresh(struct board_struct* board, enum direction dir){
    struct point* head = SNAKE_HEAD(board), *pos;
    int x = head->x, y = head->y;
    // fprintf(stderr, "dir %d | ", dir);
    switch (dir)
    {
    case left:
        if(y == 0)
            return FAIL;
        y--;
        break;
    case up:
        if(x == 0)
            return FAIL;
        x--;
        break;
    case right:
        if(y >= NUM_COLS)
            return FAIL;
        y++;
        break;
    case down:
        if(x >= NUM_ROWS)
            return FAIL;
        x++;
        break;
    
    
    default:
        return FAIL;
    }
    pos = list_last_entry(&board->snake, struct point, list);
    list_del(&pos->list);
    pos->x = x;
    pos->y = y;
    list_add(&pos->list, &board->snake);
    // debug_board(board);
    // x++;
    // if(x > NUM_COLS){
    //     x = 1;
    //     y++;
    // }
    // if(y > NUM_ROWS){
    //     y = 1;
    //     x = 1;
    // }
    return SUCCESS;
}
// \e[1;1H

void draw_point(struct point* pos, char character){
    static char draw_pos[] = {0x1b, 0x5b, 0x31, 0x3b, 0x31, 0x48, 0};
    static char format[] = "%s";
    draw_pos[2] = '0' + pos->x;
    draw_pos[4] = '0' + pos->y;
    format[2] = character;
    printf(format, draw_pos);
    printf("x");
    fprintf(stderr, "x %d y %d; ", pos->x, pos->y);
}

void draw(struct board_struct *board){
    struct point* p;
    list_for_each_entry(struct point, p, &board->snake, list){
        draw_point(p, 'x');
    }
    // list_for_each_entry(struct point, p, &board->foods, list){
    //     draw_point(p, 'o');
    // }
}

int main(int argc, char** argv){
    int ret;
    struct board_struct board;
    struct board_struct* bp = &board;
    enum direction dir = right;
    board_init(bp);
    while(1){
        int tmp = 50000;
        clear_screen();
        dir = get_direction(dir);
        ret = refresh(bp, dir);
        if(ret){
            printf("failed\n");
            break;
        }
        draw(bp);
        while(tmp--);
    }
    return 0;
}

