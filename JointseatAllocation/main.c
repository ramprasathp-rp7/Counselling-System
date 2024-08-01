#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <termios.h>


#define STUD_SIZE 5
#define SEAT_SIZE 16
#define COU_SIZE 10

char STUD_FILE[50], SEAT_FILE[50];

#define RESET_COLOR     "\x1b[0m"
#define BLACK_COLOR     "\x1b[30m"
#define RED_COLOR       "\x1b[31m"
#define GREEN_COLOR     "\x1b[32m"
#define YELLOW_COLOR    "\x1b[33m"
#define BLUE_COLOR      "\x1b[34m"
#define MAGENTA_COLOR   "\x1b[35m"
#define CYAN_COLOR      "\x1b[36m"
#define WHITE_COLOR     "\x1b[37m"
#define COLOR_PINK "\033[38;5;207m"
#define GOLD            "\033[33;1m"
#define BOLD_RED    "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_BLUE   "\033[1;34m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_COLOR      "\x1b[1m"
#define UNDERLINE_COLOR "\x1b[4m"
#define DIM_TEXT        "\x1b[2m"
#define ITALIC_TEXT     "\x1b[3m"
#define REVERSE_FONT "\033[7m"

#define BG_BLACK    "\x1b[40m"
#define BG_RED      "\x1b[41m"
#define BG_GREEN    "\x1b[42m"
#define BG_YELLOW   "\x1b[43m"
#define BG_BLUE     "\x1b[44m"
#define BG_MAGENTA  "\x1b[45m"
#define BG_CYAN     "\x1b[46m"
#define BG_WHITE    "\x1b[47m"

struct{
  int stu_id;
  char stu_name[20];
  int mains_rank;
  int adv_rank;
  int choices[COU_SIZE];
}stud_info[STUD_SIZE],current_stud,update_stud[STUD_SIZE];

struct{
  int cou_id;
  char cou_name[15];
  char col_name[20];
  int preferences[STUD_SIZE];
  int avail_seat;
  int seat_filled;
}course_info[COU_SIZE];

//update choice list
void update_choicelist(){
  char line[10000],number[10];
  line[0]='\0';
  FILE *ptr;
  ptr= fopen("student_info.csv","w");
  for(int i=0; i< STUD_SIZE; i++){
    sprintf(number, "%d", update_stud[i].stu_id);
    strcat(line,number);
    strcat(line,",") && strcat(line,update_stud[i].stu_name);
    sprintf(number,"%d",update_stud[i].mains_rank);
    strcat(line,",") && strcat(line,number);
    sprintf(number,"%d",update_stud[i].adv_rank);
    strcat(line,",") && strcat(line,number);
    for(int j=0; j< COU_SIZE; j++){
      sprintf(number,"%d",update_stud[i].choices[j]);
      strcat(line,",") && strcat(line,number);
    }
    fprintf(ptr,"%s\n",line);
    line[0]='\0';
  }
  fclose(ptr);
}

void printHeader(char symbol) {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    int screenWidth = w.ws_col;
printf("\n");
    for (int i = 0; i < screenWidth; i++) {
        printf(MAGENTA_COLOR"%c"RESET_COLOR, symbol);
    }
    printf("\n");
}
//Title
void Title(char symbol, const char* text) {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    int screenWidth = w.ws_col;
    int textWidth = strlen(text);
    int remainingWidth = screenWidth - (2 * textWidth);
    int symbolsOnEachSide = remainingWidth / 2;
    for (int i = 0; i < symbolsOnEachSide+5; i++) {
        printf(MAGENTA_COLOR"%c"RESET_COLOR, symbol);
    }
    printf(BOLD_COLOR"%s"RESET_COLOR, text);
    for (int i = 0; i < symbolsOnEachSide+3; i++) {
        printf(MAGENTA_COLOR"%c"RESET_COLOR, symbol);
    }
    printf("\n");
}

//Centrejust
void printCenteredText(const char* text) {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    int screenWidth = w.ws_col;
    int textWidth = strlen(text);
    int paddingWidth = (screenWidth - textWidth) / 2;
    for (int i = 0; i < paddingWidth+4; i++) {
        printf(" ");
    }
    printf("%s\n", text);
}
void printIndianFlag() {
    // Define the width and height of the flag
    int width = 10;
    int height = 5;

    // Get the terminal screen width
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    int screenWidth = w.ws_col;

    // Calculate the padding for center alignment
    int padding = (screenWidth - width) / 2;

    // Print the saffron color (top) portion of the flag
    for (int i = 0; i < height / 3; i++) {
        printf("%*s", padding, "");
        for (int j = 0; j < width; j++) {
            printf("\033[48;2;255;153;51m \033[0m");
        }
        printf("\n");
    }

    // Print the white (middle) portion of the flag
    for (int i = 0; i < height / 3; i++) {
        printf("%*s", padding, "");
        for (int j = 0; j < width; j++) {
            printf("\033[48;2;255;255;255m \033[0m");
        }
        printf("\n");
    }

    // Print the green (bottom) portion of the flag
    for (int i = 0; i < height / 3; i++) {
        printf("%*s", padding, "");
        for (int j = 0; j < width; j++) {
            printf("\033[48;2;19;136;8m \033[0m");
        }
        printf("\n");
    }
}




//password masking

// Function to disable terminal input buffering
void disableInputBuffering() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

// Function to enable terminal input buffering
void enableInputBuffering() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

// Function to read a single character from the terminal without buffering
char getch() {
    char ch;
    disableInputBuffering();
    ch = getchar();
    enableInputBuffering();
    return ch;
}

// Function to read password with masking
char* getPassword() {
    const int maxLength = 100;  // Maximum password length
    char* password = (char*)malloc(maxLength * sizeof(char));  // Allocate memory for the password
    int i = 0;
    char ch;
   getch();
    printf(ITALIC_TEXT"Enter password: "RESET_COLOR);
    while ((ch = getch()) != '\n') {
        if (ch== '\r'){break;}
        if (ch == 127 && i > 0) {
            // Handle backspace
            printf("\b \b");
            i--;
        }
        else if (ch != '\b') {
            // Mask the character and store it in the password array
            printf("*");
            password[i] = ch;
            i++;
        }
    }
    password[i] = '\0';  // Null-terminate the password string
    printf("\n");
    return password;
}

// Function to check if the entered password matches the correct password
bool isPasswordCorrect(const char* enteredPassword, const char* correctPassword) {
    return strcmp(enteredPassword, correctPassword) == 0;
}



//view choicelist
void view_choicelist(control){
  int count=1;
  char flag, col_name[10],cou_name[10];
  system("clear");
  if(control==0){
    printf(BOLD_RED"User-id: %d   Name: %s\n\n"RESET_COLOR,current_stud.stu_id,current_stud.stu_name);
    printf(GOLD"Current preference list: \n\n"RESET_COLOR);
  }
  printf(BLUE_COLOR"%3s  %4s %14s %13s\n\n"RESET_COLOR,"S.NO","COU_ID","COLLEGE_NAME","COURSE_NAME");
  for(int i=0; i<COU_SIZE; i++){
    if (current_stud.choices[i]==0){
      printf(BOLD_COLOR"%3d) %4s %12s %13s\n"RESET_COLOR,count++,"-","-","-");
    }
    else if (current_stud.choices[i] > 0){
      for(int j=0; j<COU_SIZE; j++){
        if(current_stud.choices[i] == course_info[j].cou_id){
          strcpy(col_name,course_info[j].col_name);
          strcpy(cou_name,course_info[j].cou_name);
        }
      }
      printf(BOLD_COLOR"%3d) %6d %12s %12s\n"RESET_COLOR,count++,current_stud.choices[i],col_name,cou_name);
    }
  }
}

void upper(char *s){
    for(int i=0; s[i]!='\0'; i++){
        if(s[i]>=97 && s[i]<=122){
            s[i]-=32;
        }
    }
}

//search course by college
int display_course_by_college(char *name){
  char colleges[][5]={"IIT","NIT"};
  int flag=0;
  for(int i=0; i<2; i++){
    if(strcmp(name,colleges[i])==0){
      flag=1;
      break;
    }
  }
  if (flag==0){
    printf(RED_COLOR"\n%s College branch doesnt exist!!"RESET_COLOR,name);
    sleep(2);
    return 0;
  }
  printf(BLUE_COLOR"\n%6s %16s %15s %17s\n\n"RESET_COLOR,"COU_ID","College_Name","Course_Name","Seats_Available");
  for( int i=0; i<COU_SIZE; i++){
    if(strncmp(name,course_info[i].col_name,3)==0){
      printf(BOLD_COLOR"%5d %14s %15s %11d\n"RESET_COLOR,course_info[i].cou_id,course_info[i].col_name,course_info[i].cou_name,course_info[i].avail_seat);
    }
  }
  printf(RED_COLOR"\nKindly Note the COU_ID for choice filling"RESET_COLOR);  
  return 1;
}

//search course by name
int display_course_by_name(char *name, char courses[][10], int size_cou){
  int check=0;
  for(int i=0; i<size_cou; i++){
    if(strcmp(name,courses[i])==0){
      check =1;
      strcpy(name,courses[i]);
      break;
    }
  }
  if(check==0){
    printf("\nNo courses available with the name %s",name);
    sleep(2);
    return 0;
  }
  printf(BLUE_COLOR"\n%6s %16s %15s %17s\n\n","COU_ID","College_Name","Course_Name","Seats_Available"RESET_COLOR);
  for( int i=0; i<COU_SIZE; i++){
    if(strcmp(name,course_info[i].cou_name)==0){
      printf(BOLD_COLOR"%5d %14s %15s %11d\n"RESET_COLOR,course_info[i].cou_id,course_info[i].col_name,course_info[i].cou_name,course_info[i].avail_seat);
    }
  }
  printf(BOLD_RED"\nKindly Note the COU_ID for choice filling\n"RESET_COLOR);  
  return 1;
}

//displays course details with course code
void display_course(){
  int choices,check=0,size_cou;
  char cont,name[20];
  char courses[][10]={"CSE","MECH","EEE","ECE","IT","CIVIL"};
  size_cou= 6;
  flag:
  system("clear");
  printHeader('=');
  printf(GOLD"\nSearch Courses: \n"RESET_COLOR);
  printf(BLUE_COLOR"\n1."RESET_COLOR);
  printf(GREEN_COLOR"Search by Course Name"RESET_COLOR);
  printf(BLUE_COLOR"\n2."RESET_COLOR);
  printf(GREEN_COLOR"Search by College branch\n"RESET_COLOR);
  printf(ITALIC_TEXT"\nEnter option: "RESET_COLOR);
  scanf("%d",&choices);
  printHeader('=');
  
  switch(choices){
    case 1: system("clear");
            printf(ITALIC_TEXT"\nEnter Course Name "RESET_COLOR);printf(DIM_TEXT"Eg:- CSE"RESET_COLOR);printf(" : ");
            scanf("\n%s",name);
            upper(name);
            check= display_course_by_name(name,courses,size_cou);
            break;
    case 2: system("clear");
            printf(ITALIC_TEXT"\nEnter college Branch "RESET_COLOR);printf(DIM_TEXT"Eg:-IIT"RESET_COLOR);printf(" : ");
            scanf("\n%s",name);
            upper(name);
            check= display_course_by_college(name);
            break;
    default: 
            printf(BOLD_RED"\nWarning: Invalid Option!!\n"RESET_COLOR);
            sleep(2);
            goto flag;
  }
  printf("\nWant to see more courses? (Y/N): ");
  scanf("\n%c",&cont);
  if(cont == 'Y' || cont == 'y'){
    goto flag;
  }
}

//updates new struct
void update_struct(){
  for(int i=0; i<STUD_SIZE; i++){
    if(current_stud.stu_id == update_stud[i].stu_id){
      update_stud[i]=current_stud;
      break;
    }
  }
}

int check_course_id(int position,int cou_id){
  //to remove previous choice at given position
  if(cou_id==0){
    current_stud.choices[position-1]= 0;
      return 1;
  }

  //to check eligibility for IIT
  for(int i=0; i<COU_SIZE; i++){
    if(cou_id == course_info[i].cou_id && (strncmp(course_info[i].col_name,"IIT",3)==0)){
      if(current_stud.adv_rank == 0){
        printf("\nYou are not eligible for IIT!!\n");
        sleep(2);
        return 1;
      }
      else break;
    }
  }
  //to check for duplicate courses in choice list
  for(int i=0; i<COU_SIZE; i++){
    if(cou_id== current_stud.choices[i]){
      printf("\nCourse is already taken!!\n");
      sleep(1);
      return -1;
    }
  }
  //to check if the course is actually available
  for(int i=0; i<COU_SIZE; i++){
    if(cou_id== course_info[i].cou_id){
      current_stud.choices[position-1]=cou_id;
      //to remove inbetween zeros on a choice list
      int *p1=current_stud.choices,count_zeros=0;
      int *p2=current_stud.choices;
      for(int i=0; i<COU_SIZE; i++){
          if(*p1==0){
              count_zeros++;
              p1++;
              continue;
          }
          else{
              *p2=*p1;
              p1++;
              p2++;
          }
      }
      while(count_zeros !=0 ){
          current_stud.choices[COU_SIZE-count_zeros]=0;
          count_zeros--;
      }
      return 1;
    }
  }
  return 0;
}

//Edit choice list
int edit_choicelist(){
  int cont=1,choice,cou_id,position;
  char flag;
  flag1:
  while(cont){
    view_choicelist(0);
    
printHeader('=');
    printf(BLUE_COLOR"1."RESET_COLOR);
    printf(GREEN_COLOR"Search Courses"RESET_COLOR);
    printf(BLUE_COLOR"\n2."RESET_COLOR);
    printf(GREEN_COLOR"Update Choicelist");
    printf(BLUE_COLOR"\n3."RESET_COLOR);
    printf(YELLOW_COLOR"Go back"RESET_COLOR);
    printf(BLUE_COLOR"\n4."RESET_COLOR);
    printf(RED_COLOR"Exit"RESET_COLOR);
    printHeader('=');
    
    printf(ITALIC_TEXT"\nEnter your choice: "RESET_COLOR);
    scanf("%d",&choice);
    
    switch(choice){
      case 1: system("clear");
              display_course();
              goto flag1;
              break;
      case 2: 
              printf(DIM_TEXT"\nNOTE: If you want to remove a Choice made,Enter the position and specify COU_ID to be '0',entries can be overwritten\n"RESET_COLOR);
              printf(ITALIC_TEXT"\nEnter position and COU_ID : "RESET_COLOR);
              scanf("%d %d",&position,&cou_id);
              if(check_course_id(position,cou_id)){
                cont= 1;
                if(cont){
                  printf(BOLD_GREEN"Successfully updated!!\n"RESET_COLOR);
                  sleep(1);
                  update_struct();
                }
              }
              else if(check_course_id(position,cou_id)==0){
                printf(BOLD_RED"\nNo Courses Matched !! \nTry Again!!\n"RESET_COLOR);
                sleep(2);
                goto flag1;
              }
              break;
      case 3: system("clear");
              update_choicelist();
              return 1;
      case 4: update_choicelist();
              sleep(1);
              exit(0);
      default: 
              printf(BOLD_RED"\nWarning: Invalid option!!\n"RESET_COLOR);
              sleep(1);
              goto flag1;
    }
  }
  return 0;
}

//View allotment details of the particular student if it exist
int view_allotment(){
  char flag;
  int alloted_seat=0,stud_id=0;
  FILE *allotment;
  allotment =fopen("allotments.txt","r");
  if(allotment==NULL){
    printf("\nSeats are not yet allotted!!\nPlease wait till seat allocation process is complete!!\n");
    printf("Go back? (Y/N): ");
    scanf("\n%c",&flag);
    if(flag == 'Y' || flag == 'y') return 1;
    else return 0;
  }
  //read allotment details
  int flag1=0;
  while(fscanf(allotment,"%d %d",&stud_id,&alloted_seat)!=EOF){
    if(stud_id == current_stud.stu_id){     
      for(int i=0; i<COU_SIZE; i++){
        if(alloted_seat == course_info[i].cou_id){
          flag1=1;
          
          printf("\nCongrats %s!! \nYou got %s in %s!!\n",current_stud.stu_name,course_info[i].cou_name,course_info[i].col_name);
         
        }
      }
    }
  }
if(flag1==0){
    printf("\nSorry You didn't get any seat!!\n");
  }
  return 0;
}

int swap_choices(){
  int pos[2]={0},temp;
  printf(ITALIC_TEXT"Enter preferences to swap: "RESET_COLOR);
  scanf("%d %d",&pos[0],&pos[1]);
  if((pos[0]<=0 || pos[0]>SEAT_SIZE) || (pos[1]<=0 || pos[1]>SEAT_SIZE)){
    printf(BOLD_RED"\nInvalid positions !!\n"RESET_COLOR);
    return 0;
  }
  else{
    temp= current_stud.choices[pos[0]-1];
    current_stud.choices[pos[0]-1] = current_stud.choices[pos[1]-1];
    current_stud.choices[pos[1]-1] = temp;
    // to terminate zeros inbetween choices
    int *p1=current_stud.choices,count_zeros=0;
    int *p2=current_stud.choices;
    for(int i=0; i<COU_SIZE; i++){
        if(*p1==0){
            count_zeros++;
            p1++;
            continue;
        }
        else{
            *p2=*p1;
            p1++;
            p2++;
        }
    }
    while(count_zeros !=0 ){
        current_stud.choices[COU_SIZE-count_zeros]=0;
        count_zeros--;
    }
    return 1;
  }
}

//module to view choice list
int reorder_choicelist(){
  int cont=1;
  char flag;
  while(cont){
    view_choicelist(0);
    printf("\nWant to reorder preferences? (Y/N): ");
    scanf("\n%c",&flag);
    if(flag == 'Y' || flag == 'y'){
      cont= swap_choices();
      if(cont){
        printf(BOLD_GREEN"Successfully updated!!\n"RESET_COLOR);
        sleep(1);
        update_struct();
      }
    }
    else cont=0;
  }
  update_choicelist();
  printf("\nGo back? (Y/N): ");
  scanf("\n%c",&flag);
    if(flag == 'Y' || flag == 'y') return 1;
    else return 0;
}
void initialize();

int update_rankList()
{
  int cont=1;
  char flag, new_studFile[50];
  FILE *files;
  while(cont){
    printf("\nWant to update rank list? (Y/N): ");
    scanf("\n%c",&flag);
    if(flag == 'Y' || flag == 'y'){
      printf("\nEnter path to updated rank list: ");
      scanf(" %[^\n]s", new_studFile);
      if (access(new_studFile, F_OK) == 0) {
        // file exists
        files = fopen("files.txt","w");
        fprintf(files, "%s\n", new_studFile);
        fprintf(files, "%s", SEAT_FILE);
        fclose(files);
      } else {
        // file doesn't exist
        printf(BOLD_RED"No file exists with the name %s\n"RESET_COLOR,new_studFile);
        sleep(1);
        continue;
      }
      if(cont){
        cont=0;
        printf(BOLD_GREEN"Successfully updated!!\n"RESET_COLOR);
        sleep(1);
        initialize();
      }
    }
    else cont=0;
  }
  printf("\nGo back? (Y/N): ");
  scanf("\n%c",&flag);
    if(flag == 'Y' || flag == 'y') return 1;
    else return 0;
}

//modifying seat availability
int modify_seat_availability(){
  int cont=1,count=1;
  int cou_id, seat_count;
  char flag;
  while(cont){
    system("clear");
    printf(BLUE_COLOR"%3s  %4s %14s %13s %12s\n\n"RESET_COLOR,"S.NO","COU_ID","COLLEGE_NAME","COURSE_NAME","SEATS_AVAIL");
    for(int i=0; i<COU_SIZE; i++){
      printf(BOLD_COLOR"%3d) %6d %12s %12s %11d\n"RESET_COLOR,count++,course_info[i].cou_id,course_info[i].col_name,course_info[i].cou_name,course_info[i].avail_seat);
    }

    if(cont==2){
      printf("\nMore changes? (Y/N): ");
      scanf("\n%c",&flag);
      if(flag == 'Y' || flag == 'y') cont=2;
      else{
        printf("\nGo back? (Y/N): ");
        scanf("\n%c",&flag);
        if(flag == 'Y' || flag == 'y') return 1;
        else return 0;
        
      }
    }
    
    printf("\nEnter COU_id and New Seat-availability: ");
    scanf("\n%d %d",&cou_id,&seat_count);
    for(int i=0; i<COU_SIZE; i++){
      if(cou_id == course_info[i].cou_id){
        cont=0;
        course_info[i].avail_seat = seat_count;
        break;
      }
    }
    if(cont==0){
      printf(BOLD_GREEN"Updated successfully\n"RESET_COLOR);
      sleep(1);
    }
    else{
      printf(BOLD_RED"NO COURSE FOUND!!\n"RESET_COLOR);
      sleep(1);
    }
    cont=2;
    FILE *ptr;
    ptr = fopen("seat_matrix.txt","w");
    for(int i=0; i<COU_SIZE; i++){
      fprintf(ptr,"%d %s %s %d\n",course_info[i].cou_id,course_info[i].col_name,course_info[i].cou_name,course_info[i].avail_seat);
    }
    fclose(ptr);
    count=1;
  }
  return 1;
}

int gale_shapley();
//Admin window
void admin_window(){
  int cont=1;
  while(cont){
    flag:
    system("clear");
    int choice;
    Title('=', "Admin Window");
    printf(COLOR_PINK"\n1."RESET_COLOR);
    printf(BOLD_GREEN"Modify Seat availability"RESET_COLOR);
    printf(COLOR_PINK"\n2."RESET_COLOR);
    printf(BOLD_GREEN"Update Rank List"RESET_COLOR);
    printf(COLOR_PINK"\n3."RESET_COLOR);
    printf(BOLD_YELLOW"Initialize allotment process"RESET_COLOR);
    printf(COLOR_PINK"\n4."RESET_COLOR);
    printf(BOLD_RED"Logout"RESET_COLOR);
    printf(ITALIC_TEXT"\n\nEnter your choice: "RESET_COLOR);
    scanf("%d",&choice);
    printHeader('=');
  
    switch(choice){
      case 1: cont= modify_seat_availability();
              break;
      case 2: cont= update_rankList();
              break;
      case 3: cont = gale_shapley();
              break;
      case 4: exit(0);
      default:
              printf(BOLD_RED"Warning: Invalid Option!!\n"RESET_COLOR);
              sleep(2);
              goto flag;
    }
  }
}

//Admin Login
void admin_log(){
  int admin_id;
  char admin_pwd[8];
  flag:
  system("clear");
  Title('=', "Admin Login");

  printf(ITALIC_TEXT"\nEnter admin id:"RESET_COLOR);
  printf(" ");
  scanf("%d",&admin_id);
  // Implementation of a security feature
  /*Only you and your invited collaborators can see Secret values in a Repl. Secrets data is encrypted with AES-256 at rest, and encryption keys are stored in a secure location that is protected by multiple layers of security. To help protect against key compromise, encryption keys are rotated regularly. Data in transit is encrypted using TLS. These measures safeguard your Secrets against breaches and vulnerabilities to ensure secure storage and protection.*/
  char* correctPassword = getenv("Admin_pwd");
  char* enteredPassword = getPassword();
  bool isCorrect = isPasswordCorrect(enteredPassword, correctPassword);
printHeader('=');  
  if (1111 == admin_id && (isCorrect)){
    printf(BOLD_GREEN"\nLogin Successfull!!\n"RESET_COLOR);
    free(enteredPassword);
    sleep(1);
    admin_window();
  }
  else{
    printf(RED_COLOR"\nLogin UnSuccesfull!! \nTry again!!\n"RESET_COLOR);
    sleep(2);
    goto flag;
  }
}

//student Window
void stud_window(){
  int cont=1;
  while(cont){
    flag:
    system("clear");
    int choice;
    Title('=', "Student Window");

    printf(CYAN_COLOR"\n1."RESET_COLOR);
    printf(BOLD_GREEN"Edit Choice List"RESET_COLOR);
    printf(CYAN_COLOR"\n2."RESET_COLOR);
    printf(BOLD_GREEN"View Finalised Choice List/ Reorder Choices"RESET_COLOR);
    printf(CYAN_COLOR"\n3."RESET_COLOR);
    printf(BOLD_YELLOW"View Allotment"RESET_COLOR);
    printf(CYAN_COLOR"\n4."RESET_COLOR);
    printf(BOLD_RED"Logout"RESET_COLOR);
    printf(ITALIC_TEXT"\n\nEnter your choice: "RESET_COLOR);
    scanf("%d",&choice);
printHeader('=');
  
    switch(choice){
      case 1: cont= edit_choicelist();
              break;
      case 2: cont = reorder_choicelist();
              break;
      case 3: cont = view_allotment(); 
              break;
      case 4: exit(0);
      default:
              printf(BOLD_RED"Warning: Invalid Option!!\n"RESET_COLOR);
              sleep(2);
              goto flag;
    }
  }
}

//Student Login
void stud_log(){
    flag:
    system("clear");
    int stud_id, found = 1;
    char stud_pwd[20];
    Title('=', "Student Login");
    printf(ITALIC_TEXT"\nEnter student id:"RESET_COLOR); printf(" ");
    scanf("%d", &stud_id);
    char* enteredPassword = getPassword();
    bool isCorrect = isPasswordCorrect(enteredPassword, stud_pwd);
    printHeader('=');
    for (int i = 0; i < STUD_SIZE; i++) {
        if ((stud_info[i].stu_id == stud_id) && (strcmp(stud_info[i].stu_name, enteredPassword) == 0)) {
            found = 0;
            current_stud = stud_info[i];
            printf(BOLD_GREEN"\nLogin successful! Welcome %s \n"RESET_COLOR, current_stud.stu_name);
            sleep(2);
            stud_window();
            break;
        }
    }

    if (found) {
        printf(RED_COLOR"\nLogin Unsuccessful!! \nRetry Login\n"RESET_COLOR);
        sleep(2);
        goto flag;
    }

    free(enteredPassword);
}

void initialize(){
  int i=0,j=0;
  char cur_line[1000];
  char *ptr;
  FILE *files;
  FILE *stud;
  files = fopen("files.txt","r");
  fgets(STUD_FILE, 50, files);
  fgets(SEAT_FILE, 50, files);
  fclose(files);
  //initialize stud_info to struct
  for (int i=0; i<50; ++i)
    {
      if (STUD_FILE[i]=='\n')
      {
        STUD_FILE[i]='\0';
        break;
      }
    }
  for (int i=0; i<50; ++i)
    {
      if (SEAT_FILE[i]=='\n')
      {
        SEAT_FILE[i]='\0';
        break;
      }
    }
  
  stud = fopen(STUD_FILE ,"r");
  if(stud == NULL){ 
    printf(BOLD_RED"\nStudent_info.csv doesn't exist"RESET_COLOR);
    exit(1);
  }
  while(fscanf(stud,"%s",cur_line)!=EOF){
    ptr =strtok(cur_line,",");
    stud_info[i].stu_id = atoi(ptr);
    ptr = strtok(NULL,",");
    strcpy(stud_info[i].stu_name,ptr);
    ptr = strtok(NULL,",");
    stud_info[i].mains_rank= atoi(ptr);
    ptr = strtok(NULL,",");
    stud_info[i].adv_rank= atoi(ptr);
    ptr = strtok(NULL,",");
    while(ptr != NULL){
      stud_info[i].choices[j]=atoi(ptr);
      j++;
      ptr = strtok(NULL,",");
    }
    i++;
    j=0;
  }
  fclose(stud);

  //initialize Course_info to struct
  i=0;
  FILE *cour;
  cour = fopen(SEAT_FILE,"r");
  if(cour==NULL){
    printf(BOLD_RED"\nSeat_matrix file does not exist\n"RESET_COLOR);
    exit(1);
  }
  while(fscanf(cour,"%d %s %s %d",&course_info[i].cou_id,course_info[i].col_name,course_info[i].cou_name,&course_info[i].avail_seat)!=EOF){
    i++;
  }
  fclose(cour);

  //creating a copy of orignal struct
  for(int i=0; i<STUD_SIZE; i++){
    update_stud[i] = stud_info[i];
  }
}

bool is_all_matched(bool *students){
  bool flag = true;
  for(int i=0; i<STUD_SIZE; i++){
    if(students[i] == true) flag=false;
  }
  return flag;
}

bool if_better(int *seat_match,bool *unmatched, int current_course, int current_rank, int ranklist){
  int count=0,j=0,last_matched;
  for(int i=0; i<STUD_SIZE; i++){
    if(current_course == seat_match[i]) count++;
  }
  
  int course_matched[count][2];
  for(int i=0; i<STUD_SIZE; i++){
    if(current_course == seat_match[i] ){
      if(ranklist==2)
      course_matched[j][0]= stud_info[i].adv_rank;
      else if (ranklist==1)
      course_matched[j][0]= stud_info[i].mains_rank;
      course_matched[j][1]=i;
      j++;
    }
  }
  // Bubble sort the course_matched array based on the first column (rank)
  for (int i = 0; i < count - 1; i++) {
    for (int j = 0; j < count - i - 1; j++) {
      if (course_matched[j][0] > course_matched[j + 1][0]) {
        // Swap ranks
        int temp_rank = course_matched[j][0];
        course_matched[j][0] = course_matched[j + 1][0];
        course_matched[j + 1][0] = temp_rank;
  
        // Swap student indices
        int temp_index = course_matched[j][1];
        course_matched[j][1] = course_matched[j + 1][1];
        course_matched[j + 1][1] = temp_index;
      }
    }
  } 
  last_matched = course_matched[count-1][0];
  if(last_matched < current_rank || current_rank==0){
    return false;
  }
  else{
    unmatched[course_matched[count-1][1]]= true;
    seat_match[course_matched[count-1][1]]= -1;
    return true;
  }
}

int gale_shapley(){
  //for college preferences
  for(int i=0; i<STUD_SIZE; i++){
    for(int j=0; j<COU_SIZE; j++){
      for(int k=0; k<COU_SIZE; k++){
        if(stud_info[i].adv_rank>0 && stud_info[i].choices[j] == course_info[k].cou_id){
          if(strncmp(course_info[k].col_name,"IIT",3)==0){
            course_info[k].preferences[stud_info[i].adv_rank-1]= stud_info[i].stu_id;
          }
        }
        if(stud_info[i].mains_rank>0 && stud_info[i].choices[j] == course_info[k].cou_id){
          if(strncmp(course_info[k].col_name,"NIT",3)==0){
            course_info[k].preferences[stud_info[i].mains_rank-1]= stud_info[i].stu_id;
          }
        }
      }
    }                     
  }
  //to verify course preferences
  //     for(int i=0; i<COU_SIZE; i++){
  //   printf("%d %s %s ",course_info[i].cou_id,course_info[i].col_name,course_info[i].cou_name);
  //   for(int j=0; j<STUD_SIZE; j++){
  //     printf("%d ",course_info[i].preferences[j]);
  //   }
  //   printf("\n");
  // }
  // printf("\n");

  
  //Gale Shapely
  int seat_match[STUD_SIZE],cou_index=0;
  bool unmatched[STUD_SIZE];

  for (int i = 0; i < STUD_SIZE; i++) {
    seat_match[i] = -1;
    unmatched[i] = true;
  }

  while(true){
    // to check if everyone is matched
    if(is_all_matched(unmatched)){
      break;
    }
    
    for(int i=0; i<STUD_SIZE; i++){
      if(seat_match[i]==-1){
        //to assign students to null university if no match
        if(stud_info[i].choices[cou_index]==0 || (stud_info[i].choices[cou_index]>=22000 && stud_info[i].adv_rank==0) || (stud_info[i].choices[cou_index]<22000 && stud_info[i].mains_rank==0)){
          seat_match[i]=0;
          unmatched[i]=false;
        }
        else{
          for(int j=0; j<COU_SIZE; j++){
            if(stud_info[i].choices[cou_index]==course_info[j].cou_id){
              // to match if the seats are available
              if(course_info[j].avail_seat > course_info[j].seat_filled){
                seat_match[i]= course_info[j].cou_id;
                unmatched[i]=false;
                course_info[j].seat_filled++;
                break;
              }
              //to match if the current seat is matched better
              else if(course_info[j].avail_seat == course_info[j].seat_filled){
                int ranklist= course_info[j].cou_id > 22000 ? 2:1;
                int current_rank= ranklist < 2 ? stud_info[i].mains_rank : stud_info[i].adv_rank;
                if(if_better(seat_match,unmatched, course_info[j].cou_id,current_rank, ranklist)){
                  unmatched[i]=false;
                  seat_match[i]= course_info[j].cou_id;
                  //cou_index=0;
                }
              }
            }
          }
        }
      }
    }
    cou_index++;
  }
  system("clear");
  printf(GOLD"Alloted Details: \n\n"RESET_COLOR);
  printf(BLUE_COLOR"%s %10s %8s %15s %16s\n\n"RESET_COLOR,"STUD_ID","STUD_NAME","COU_ID","ALLOTED_COURSE","ALLOTED_COLLEGE");
  for(int i=0; i<STUD_SIZE; i++){
    printf(BOLD_COLOR"%d %12.9s "RESET_COLOR,stud_info[i].stu_id,stud_info[i].stu_name);
    for(int j=0; j<COU_SIZE;j++){
      if(seat_match[i]==course_info[j].cou_id){
        printf(BOLD_COLOR"%9d %11.3s %14s\n"RESET_COLOR,seat_match[i],course_info[j].cou_name,course_info[j].col_name);
      }
      else if(seat_match[i]==0){
        printf(BOLD_COLOR"%9s %10s %13s\n"RESET_COLOR,"None","-","-");
        break;
      }
    }
  }

  printf(GOLD"\nSeat availability for upcomming rounds: "RESET_COLOR);
  printf(BLUE_COLOR"\n%6s %16s %15s %17s\n\n","COU_ID","College_Name","Course_Name","Seats_Available"RESET_COLOR);
  for( int i=0; i<COU_SIZE; i++){
    if(course_info[i].seat_filled-course_info[i].avail_seat==0){
      printf(DIM_TEXT"%5d %14s %15s %11d\n"RESET_COLOR,course_info[i].cou_id,course_info[i].col_name,course_info[i].cou_name,course_info[i].seat_filled-course_info[i].avail_seat);
    }
    else{
      printf(BOLD_COLOR"%5d %14s %15s %11d\n"RESET_COLOR,course_info[i].cou_id,course_info[i].col_name,course_info[i].cou_name,course_info[i].avail_seat-course_info[i].seat_filled);
    }
  }

  FILE *fp;
  fp = fopen("allotments.txt","w");
  for(int i=0; i<STUD_SIZE; i++){
    fprintf(fp,"%d %d\n",stud_info[i].stu_id,seat_match[i]);
  }
  fclose(fp);
  return 0;
}

int main(){
  initialize();
  int choice;
  flag:
  system("clear");
  //displayDateTime();
  printHeader('=');
  printCenteredText(BOLD_COLOR"JoSSA-Counselling 2023"RESET_COLOR);
  printf("\n");
  printIndianFlag();
  printHeader('=');
  printf("\n");
  
  printCenteredText(CYAN_COLOR"Welcome to the Admission and "RESET_COLOR);
  printCenteredText(CYAN_COLOR"eCounselling Services for Session 2023!"RESET_COLOR);
  
  printf("\n");
  printf(RED_COLOR"1."RESET_COLOR);
  printf(BOLD_COLOR" Admin Login\n"RESET_COLOR);
  printf(RED_COLOR"2."RESET_COLOR);
  printf(BOLD_COLOR" Student Login\n"RESET_COLOR);
  printf(RED_COLOR"3."RESET_COLOR);
  printf(BOLD_COLOR" Exit\n"RESET_COLOR);
  //gale_shapley();
  printf(ITALIC_TEXT"\nEnter your choice: "RESET_COLOR);
  scanf("%d",&choice);

printHeader('=');  
  switch(choice){
    case 1: admin_log(); break;
    case 2: stud_log(); break;
    case 3: exit(0);
    default: 
            printf(BOLD_RED"Warning: Invalid Option!!\n"RESET_COLOR);
            sleep(2);
            goto flag;
  }
}