#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <ncurses.h>


int work_flag = 1;
int fb, x, y, xstep, ystep;
struct fb_var_screeninfo info;
size_t fb_size, map_size, page_size;
uint32_t *ptr, color;
int up = 0,left = 0,down = 0,right = 0, ch = 0;
void handler(int none)
{
  work_flag = 0;
}


void pot_1(void){
  while(work_flag) {

    ptr[y * info.xres_virtual  + x] = color;
    if (up == 1){

      xstep = 0;
      if ( 0 >= (y + ystep)) {
        ystep = -ystep;
      }else {
        ystep = -1;
      }
      y += ystep;
    }
    else if (down == 1){
      xstep = 0;
       if ( info.yres <= (y + ystep)) {
        ystep = -ystep;
      }else {
        ystep = 1;
      }
      y += ystep;
    }
    else if (left == 1){
      if ( info.xres <= (x - xstep)) {
        xstep = -xstep;
      }else {
        xstep = -1;
      }
      x += xstep;
    }
    else if (right == 1){
      if ( info.xres <= abs((x + xstep))) {
        xstep = -xstep;
      }else {
        xstep = 1;
      }
      x += xstep;
    } else{
      x += xstep;
      y += ystep;
    }
    color++;
    usleep(2200);

  }
}
int main(int argc, char *argv[])
{


  signal(SIGINT, handler);
  
  color = 0x00000000;
  x = y = 100;
  xstep = 1;
  ystep = 0;
  page_size = sysconf(_SC_PAGESIZE);
  
  if ( 0 > (fb = open("/dev/fb0", O_RDWR))) {
    perror("open");
    return __LINE__;
  }

  if ( (-1) == ioctl(fb, FBIOGET_VSCREENINFO, &info)) {
    perror("ioctl");
    close(fb);
    return __LINE__;
  }

  fb_size = sizeof(uint32_t) * info.xres_virtual * info.yres_virtual;
  map_size = (fb_size + (page_size - 1 )) & (~(page_size-1));

  ptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
  if ( MAP_FAILED == ptr ) {
    perror("mmap");
    close(fb);
    return __LINE__;
  }
  initscr();

  noecho();
  keypad(stdscr,TRUE);
  

  pthread_t mat_two;

  pthread_create(&mat_two, NULL, pot_1, NULL);    
  while (work_flag && ch != 'q'){
    ch = getch();
   switch(ch){
             case KEY_LEFT:
                left = 1;
                up = 0;
                down = 0;
                right = 0;
           
                break;
              case KEY_RIGHT:
                right = 1;
                left = 0;
                down = 0;
                up = 0;
                break;
              case KEY_DOWN:
               down = 1;
               right = 0;
               up = 0;
               left = 0;
               break;
              case KEY_UP:
               up = 1;
               left = 0;
               right = 0;
               down = 0;
               default:
                 break;
    }
  }
  endwin();
              
  munmap(ptr, map_size);
  close(fb);
  return 0;
}