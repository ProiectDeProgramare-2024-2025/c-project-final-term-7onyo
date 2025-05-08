#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

#define MAX_USERS 100
#define MAX_NAME_LEN 50
#define LINE_LEN 200



#define RESET   "\033[0m"        
#define BLACK   "\033[30m"       
#define RED     "\033[31m"       
#define GREEN   "\033[32m"       
#define YELLOW  "\033[33m"       
#define BLUE    "\033[34m"       
#define MAGENTA "\033[35m"       
#define CYAN    "\033[36m"       
#define WHITE   "\033[37m"       
#define BOLD    "\033[1m"        


//Structs
typedef struct 
{
    char *name;
    float score;
}user;

user *createUser(char *name)
{
    user *u = (user *) malloc(sizeof(user));
    u->name = (char *) malloc(strlen(name) + 1);
    strcpy(u->name, name);
    u->score = 0;

    return u;
}

typedef struct
{
    char *components[6];
    int correctAnswer;
    bool answered; 
}question;


question *createQuestion(char **components, int correctAnswer)
{
    question *q = (question *) malloc(sizeof(question));

    for(int i = 0; i < 6; i++)
    {
        q->components[i] = (char *) malloc(strlen(components[i]) + 1);
        strcpy(q->components[i], components[i]);
    }

    q->correctAnswer = correctAnswer;

    q->answered = false;

    return q;
}
//




// Global Data
char **leaderboard = NULL;

user *currentUser; 
question *questions[15] = {NULL};

int selectedQuestionIndex = -1;

char *components[3]; //for 50/50 question
char *correctAsnwer; //for 50/50 question

char header0[] = "\033[32mMAIN\033[0m \033[33mMENU\033[0m";
char *options0[] = {"Start the game", "Leaderboard"};

char header1[] = "GAME";
// char *original_options1[] =
// {
//     "Question1", "Question2", "Question3", "Question4", "Question5", 
//     "Question6", "Question7", "Question8", "Question9", "Question10", 
//     "Question11", "Question12", "Question13", "Question14", "Question15"
// };

char *original_options1[] =
{
    "\033[34mQuestion\033[0m\033[36m1\033[0m",   
    "\033[31mQuestion\033[0m\033[32m2\033[0m",   
    "\033[33mQuestion\033[0m\033[34m3\033[0m",   
    "\033[35mQuestion\033[0m\033[37m4\033[0m",   
    "\033[36mQuestion\033[0m\033[31m5\033[0m",   
    "\033[32mQuestion\033[0m\033[33m6\033[0m",   
    "\033[34mQuestion\033[0m\033[35m7\033[0m",   
    "\033[37mQuestion\033[0m\033[36m8\033[0m",   
    "\033[31mQuestion\033[0m\033[34m9\033[0m",   
    "\033[32mQuestion\033[0m\033[33m10\033[0m",  
    "\033[33mQuestion\033[0m\033[35m11\033[0m",  
    "\033[35mQuestion\033[0m\033[37m12\033[0m",  
    "\033[36mQuestion\033[0m\033[31m13\033[0m",  
    "\033[32mQuestion\033[0m\033[34m14\033[0m",  
    "\033[34mQuestion\033[0m\033[36m15\033[0m"   
};



char *options1[15];

char header2[] = "QUESTION";

char header2_ff[] = "\033[32m50\033[0m \033[33m/\033[0m \033[35m50\033[0m";

char header3[] = "LEADERBOARD";
char *options3[] = {"1 -- onyo", "2 -- daNi", "3 -- Ez25"};

char header4[] = "\033[33mUSER\033[0m \033[35mHISTORY\033[0m";
char *options4[] = 
{
    "Question1: 1p", "Question2: 0p", "Question3: 1p", "Question4: 0.5p", "Question5: 1p", 
    "Question6: 1p", "Question7: 0p", "Question8: 0p", "Question9: 0p", "Question10: 1p", 
    "Question11: 0p", "Question12: 0p", "Question13: 0p", "Question14: 0.5p", "Question15: 0.5p",
    "Total Score: 6.5p"
};

char header5[] = "\033[33mFINAL\033[0m \033[35mSCREEN\033[0m";
char *options5[3];
//




//Function Prototypes
void selectRandoms(int exception, int *randNr1, int *randNr2);
void appendScoreToQuestion(int choice);
bool checkAllAnswered();
void finalScreen();
void setColor(int textColor, int backgroundColor);
void init();
void displayMenu(int currentPos, int n, char *options[], char header[]);
void menu(int n, char *options[], char header[]);

void appendRun(char *userName, float score, char *time);
int numberOfLines(char *fileName);


int find_user_index(user users[], int count, const char* name);
   

int compare_users(const void* a, const void* b); 

char **getUserRuns(const char *username, int *countOut);

//




void selectRandoms(int exception, int *randNr1, int *randNr2) 
{
  
    int min = 1;
    int max = 4;

    srand(time(0));
  
    while(1)
    {
        *randNr1 = rand() % (max - min + 1) + min;
        *randNr2 = rand() % (max - min + 1) + min;
        if(*randNr1 != *randNr2)
            if(*randNr1 != (exception + 1) && *randNr2 != (exception + 1))
                return;
    }
    
}


void appendScoreToQuestion(int choice)
{
    int size = 5;
    if(choice == 1)
        size = 3;

    char *aux = (char *) malloc(strlen(options1[selectedQuestionIndex]) + size);
    strcpy(aux, options1[selectedQuestionIndex]);

    if(choice == 0)
        strcat(aux, " 0.5P");
        
    else if(choice == 1)
        strcat(aux, " 1P");
        
    options1[selectedQuestionIndex] = (char *) malloc(strlen(options1[selectedQuestionIndex]) + size);
    strcpy(options1[selectedQuestionIndex], aux);
    free(aux);

    
    
}


bool checkAllAnswered()
{
    for(int i = 0; i < 15; i++)
        if(questions[i]->answered == false)
            return false;
    return true;
}


void finalScreen()
{
    options5[0] = (char *) malloc(strlen(currentUser->name) + 11); //Username:
    options5[1] = (char *) malloc(12); //Score:
    options5[2] = (char *) malloc(strlen("Back to Main Menu") + 1); //Main Menu

    char *strScore = (char *) malloc(3);
    sprintf(strScore, "%.1f", currentUser->score);
    
    strcpy(options5[0], "Username: "); strcat(options5[0], currentUser->name);
    strcpy(options5[1], "Score: "); strcat(options5[1], strScore);
    strcpy(options5[2], "Back to Main Menu");



    //Local Time
    struct tm* ptr;
    time_t t;
    t = time(NULL);
    ptr = localtime(&t); 
    char *s = (char *) malloc(strlen(asctime(ptr)) + 1);
    strcpy(s, asctime(ptr));
    //

    appendRun(currentUser->name, currentUser->score, s);

    menu(3, options5, header5);
}


void setColor(int textColor, int backgroundColor)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (backgroundColor << 4) | textColor);
}


void init()
{
    char runsLine[LINE_LEN];
    user users[MAX_USERS]; 
    int userCount = 0;

    FILE* file = fopen("runs.csv", "r");
    if (!file) {
        printf("Error opening runs.csv\n");
        return;
    }

    fgets(runsLine, sizeof(runsLine), file); // skip header

    while (fgets(runsLine, sizeof(runsLine), file)) 
    {
        char* token;
        char nameBuf[MAX_NAME_LEN];
        float score;

        token = strtok(runsLine, ","); // RunId
        if (!token) continue;

        token = strtok(NULL, ","); // Username
        if (!token) continue;
        strncpy(nameBuf, token, MAX_NAME_LEN);
        nameBuf[MAX_NAME_LEN - 1] = '\0';

        token = strtok(NULL, ","); // Score
        if (!token) continue;
        score = atof(token);

        int index = find_user_index(users, userCount, nameBuf);
        if (index == -1) {
            if (userCount >= MAX_USERS) {
                printf("Max users reached, skipping extra entries\n");
                break;
            }
            users[userCount].name = (char*) malloc(MAX_NAME_LEN);
            if (!users[userCount].name) {
                printf("Memory allocation failed for username\n");
                exit(1);
            }
            strncpy(users[userCount].name, nameBuf, MAX_NAME_LEN);
            users[userCount].name[MAX_NAME_LEN - 1] = '\0';
            users[userCount].score = score;
            userCount++;
        } else {
            if (score > users[index].score)
                users[index].score = score;
        }
    }

    fclose(file);

    // Sort users by score descending
    qsort(users, userCount, sizeof(user), compare_users);

    // Allocate leaderboard
    leaderboard = (char**) malloc(userCount * sizeof(char*));
    if (!leaderboard) {
        printf("Failed to allocate memory for leaderboard.\n");
        exit(1);
    }

    for (int i = 0; i < userCount; ++i) {
        leaderboard[i] = (char*) malloc(100);
        if (!leaderboard[i]) {
            printf("Memory allocation failed for leaderboard entry\n");
            exit(1);
        }
        snprintf(leaderboard[i], 100, "%.1fP - %s", users[i].score, users[i].name);
        //snprintf(leaderboard[i], 100, "%s%.1fP%s - %s%s", GREEN, users[i].score, RESET, BLUE, users[i].name);
    }


   







    for (int i = 0; i < 15; ++i) 
    {
        options1[i] = original_options1[i];
    }


    //setColor(1, 7);
    system("cls");
    //printf("%s", "Please enter your name(CaSe SeNsItIvE, \"primary key\" that will be used to identify you among the other users): ");


    printf("%sPlease enter your %sname%s (%sCaSe SeNsItIvE%s, %s\"primary key\"%s %sthat will be used to identify you%s %samong the other users%s): ",
       YELLOW, GREEN, RESET,        
       RED, RESET,                 
       CYAN, RESET,                
       MAGENTA, RESET,             
       BLUE, RESET);               



    FILE *questionsFile = fopen("questions.csv", "r");
    if(questionsFile == NULL)
    {
        printf("COULD NOT OPPEN THE CSV FILE");
        exit(1);
    }
    char line[1000];

    int posInQuestions = 0;

    fgets(line, sizeof(line), questionsFile);
    while(fgets(line, sizeof(line), questionsFile))
    {
        char *p = strtok(line, ",");
        int posInLine = 0;
        
        char *components[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
        int correctAnswer = -1;
        
        while(p)
        {
            switch(posInLine)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                {
                    //Statement + 4 Answers + 50/50 Option
                    components[posInLine] = (char *) malloc(strlen(p) + 1);
                    strcpy(components[posInLine], p);

                    if(posInLine == 4)
                    {
                        components[posInLine + 1] = (char *) malloc(14 + 1);
                        strcpy(components[posInLine + 1], "50 / 50 Option");

                    }
                    break;
                }
                    
                case 5:
                {
                    //CorrectAnswer
                    correctAnswer = atoi(p);
                    break;
                }
                    
                default:
                    break;
            }
            p = strtok(NULL, ",");
            posInLine++;
        }
        
        questions[posInQuestions] = createQuestion(components, correctAnswer);

        for(int i = 0; i < 6; i++) 
            free(components[i]); 
        posInQuestions++;
    }
    fclose(questionsFile);
}


void displayMenu(int currentPos, int n, char *options[], char header[])
{
    system("cls");  
    // printf("%s, %s!\n", "Hi there", currentUser->name);
    // printf("%s\n", "Welcome to Who Wants to Be a Millionaire?!");
    // printf("%s\n", "Get ready to test your knowledge and climb your way to the top prize.");
    // printf("%s\n\n", "Arrow Up -- Move up | Arrow Down -- Move down | E -- Select option | Backspace -- Go back | Escape -- Exit");
    // printf("%s\n\n", header);


    printf("%sHi there%s, %s%s%s!\n", BLUE, RESET, GREEN, "dani", RESET);

    printf("%sWelcome%s to %sWho Wants to Be a Millionaire?%s\n", YELLOW, RESET, BLUE, RESET);

    printf("%sGet ready to %stest your knowledge%s and climb your way to the %stop prize%s.\n", CYAN, RESET, CYAN, MAGENTA, RESET);

    printf("%sArrow Up%s -- %sMove up%s | "
       "%sArrow Down%s -- %sMove down%s | "
       "%sE%s -- %sSelect option%s | "
       "%sBackspace%s -- %sGo back%s | "
       "%sEscape%s -- %sExit%s\n\n",
       MAGENTA, RESET, GREEN, RESET,
       MAGENTA, RESET, GREEN, RESET,
       CYAN, RESET, CYAN, RESET,
       RED, RESET, RED, RESET,
       YELLOW, RESET, YELLOW, RESET);


    printf("%s\n\n", header);

    

      

    for(int i = 0; i < n; i++)
        if(currentPos == i)
            printf(">> %s\n", options[i]); 
        else
            if (options[i] != NULL)
                printf("%s\n", options[i]);
            else
                printf("[empty]\n");
            //printf("%s\n", options[i]);       
}


void menu(int n, char *options[], char header[])
{
    int currentPos = 0;
    displayMenu(currentPos, n, options, header);
    while(1)
    {
        if(GetAsyncKeyState(VK_UP) & 0b1)
        {
            if(currentPos != 0)
            {
                currentPos--;
                displayMenu(currentPos, n, options, header);
            }
            else if(currentPos == 0)
            {
                currentPos = n - 1;
                displayMenu(currentPos, n, options, header);    
            }
        }
        else 
            if(GetAsyncKeyState(VK_DOWN) & 0b1)
            {
                if(currentPos != (n - 1))
                {
                    currentPos++;
                    displayMenu(currentPos, n, options, header);
                }
                else if(currentPos == (n - 1))
                {
                    currentPos = 0;
                    displayMenu(currentPos, n, options, header); 
                }
            }
            else 
                if(GetAsyncKeyState(0x45) & 0b1)
                {
                    if(header == header0)
                    {
                        if(currentPos == 0)
                            menu(15, options1, header1);
                        else if(currentPos == 1 && numberOfLines("runs.csv") >= 2)
                            menu(numberOfLines("users.csv") - 1, leaderboard, header3);    
                    }

                    else if(header == header1 && questions[currentPos]->answered == false)
                    {
                        selectedQuestionIndex = currentPos;
                        menu(6, questions[currentPos]->components, header2);
                    }

                    else if(header == header2)
                    {
                        if((currentPos == (questions[selectedQuestionIndex]->correctAnswer + 1)) && questions[selectedQuestionIndex]->answered == false)
                        {
                            currentUser->score += 1;
                            questions[selectedQuestionIndex]->answered = true;

                            
                            if(checkAllAnswered())
                                finalScreen();
                                
                            else
                            {
                                appendScoreToQuestion(1);
                                menu(15, options1, header1);
                            }
                        }
                        else if(currentPos == 5)
                        {
                            int *randNr1 = (int *) malloc(sizeof(int));
                            int *randNr2 = (int *) malloc(sizeof(int));
                            selectRandoms(questions[selectedQuestionIndex]->correctAnswer, randNr1, randNr2);

                            free(questions[selectedQuestionIndex]->components[*randNr1]);
                            free(questions[selectedQuestionIndex]->components[*randNr2]);
                            free(questions[selectedQuestionIndex]->components[5]);

                            questions[selectedQuestionIndex]->components[*randNr1] = NULL;
                            questions[selectedQuestionIndex]->components[*randNr2] = NULL;
                            questions[selectedQuestionIndex]->components[5] = NULL;
                            

                            free(randNr1);
                            free(randNr2);

                        
                            correctAsnwer = (char *) malloc(strlen(questions[selectedQuestionIndex]->components[questions[selectedQuestionIndex]->correctAnswer + 1]) + 1);
                            strcpy(correctAsnwer, questions[selectedQuestionIndex]->components[questions[selectedQuestionIndex]->correctAnswer + 1]);


                            int posInNewComponents = 0;
                            for(int i = 0; i < 6; i++)
                            {
                                if(questions[selectedQuestionIndex]->components[i] != NULL)
                                {
                                    components[posInNewComponents] = (char *) malloc(strlen(questions[selectedQuestionIndex]->components[i]) + 1);
                                    strcpy(components[posInNewComponents], questions[selectedQuestionIndex]->components[i]);
                                    posInNewComponents++;
                                }
                            }

                            menu(3, components, header2_ff);
                            
                        
                        }
                        else if(currentPos != (questions[selectedQuestionIndex]->correctAnswer + 1) && currentPos != 0)
                            finalScreen();
                            
                    }
                    else if(header == header2_ff)
                    {
                        
                        if(strcmp(components[currentPos], correctAsnwer) == 0)
                        {
                            if(questions[selectedQuestionIndex]->answered == false)
                            {
                                currentUser->score += 0.5;
                                questions[selectedQuestionIndex]->answered = true;

                                for(int i = 0; i < 3; i++)
                                    free(components[i]);
                                free(correctAsnwer);

                                
                                if(checkAllAnswered())
                                    finalScreen();
                
                                else
                                {
                                    appendScoreToQuestion(0);
                                    menu(15, options1, header1);
                                }

                                
                                
                            }
                        }
                        else if(currentPos != 0)
                            finalScreen();
                    }

                    else if(header == header3)
                    {
                        char *aux = (char *) malloc(strlen(leaderboard[currentPos]) + 1);
                        strcpy(aux, leaderboard[currentPos]);

                        char *p = strtok(aux, " ");
                        int cnt = 0;

                        while(p)
                        {
                            if(cnt == 2)
                            {
                                int count;
                                char **runs = getUserRuns(p, &count);

                                menu(count, runs, header4);


                                break;
                            }
                            p = strtok(NULL, " ");
                            cnt++;
                        }
                       
                    }

                    

                    else if(header == header5)
                    {
                        if(currentPos == 2)
                        {
                            currentUser->score = 0;
                            init();
                            menu(2, options0, header0);   
                        }
                        
                    }
                }
                else 
                    if(GetAsyncKeyState(VK_BACK) & 0b1)
                    {
                        // if(header == header1)
                        //     menu(2, options0, header0);

                        //else
                         if(header == header3)
                            menu(2, options0, header0);

                        else if(header == header4)
                            menu(numberOfLines("users.csv") - 1, leaderboard, header3);
                    }
                    else 
                        if(GetAsyncKeyState(VK_ESCAPE) & 0b1)
                            exit(0);
    }
}






void createUsersCsv()
{
    FILE *users = fopen("users.csv", "w");
    fprintf(users, "%s\n", "Username");
    fclose(users);
}

void createRunsCsv()
{
    FILE *runs = fopen("runs.csv", "w");
    fprintf(runs, "%s,%s,%s,%s\n", "RunId", "Username", "Score", "Time");
    fclose(runs);
}

int checkUserNameInFile(char *userName)  
{
    //return pos if exsits
    //return -1 if the user doesnt exist

    FILE *users = fopen("users.csv", "r");
    
    
    
    int posInFile = 0;
    char line[1000];
    while(fgets(line, sizeof(line), users))
    {
        char *data = strtok(line, "\n");
        while(data)
        {
            if(strcmp(data, userName) == 0)
                return posInFile;
            break;
        }
        posInFile++;
    }
    fclose(users);

    return -1;

}

void appendUser(char *userName)
{
    FILE *users = fopen("users.csv", "a");

    
    
    fprintf(users, "%s\n", userName);
    fclose(users);

}

void appendRun(char *userName, float score, char *time)
{
    FILE *runs = fopen("runs.csv", "a+");

    char line[1000];
    int id = 0;
    while(fgets(line, sizeof(line), runs))
    {
        id++;
    }

    char *formatedScore = (char *) malloc(3);
    sprintf(formatedScore, "%.1f", score);

    fprintf(runs, "%d,%s,%s,%s", id, userName, formatedScore, time);


    fclose(runs);
}

int numberOfLines(char *fileName)
{
    FILE *file = fopen(fileName, "r");
    
    int n = 0;
    char line[1000];
    while(fgets(line, sizeof(line), file))
    {
        n++;
    }
    fclose(file);

    return n;
}

int find_user_index(user users[], int count, const char* name) 
{
    for (int i = 0; i < count; ++i)
        if (strcmp(users[i].name, name) == 0)
            return i;
    return -1;
}

int compare_users(const void* a, const void* b) 
{
    float scoreA = ((user*)a)->score;
    float scoreB = ((user*)b)->score;
    return (scoreB > scoreA) - (scoreB < scoreA); 
}



char **getUserRuns(const char *username, int *countOut) 
{
    FILE *file = fopen("runs.csv", "r");
    if (!file) 
    {
        printf("Error opening runs.csv\n");
        *countOut = 0;
        return NULL;
    }

    char line[LINE_LEN];
    int count = 0;

    // Skip header
    fgets(line, LINE_LEN, file);

    // First pass: count
    while (fgets(line, LINE_LEN, file)) {
        char copy[LINE_LEN];
        strncpy(copy, line, LINE_LEN);
        copy[LINE_LEN - 1] = '\0';

        char *token = strtok(copy, ","); // RunId
        token = strtok(NULL, ",");       // Username

        if (token && strcmp(token, username) == 0) {
            count++;
        }
    }

    rewind(file); // Reset file pointer
    fgets(line, LINE_LEN, file); // Skip header again

    char **runs = (char**) malloc(count * sizeof(char *));
    int index = 0;

    while (fgets(line, LINE_LEN, file)) {
        char copy[LINE_LEN];
        strncpy(copy, line, LINE_LEN);
        copy[LINE_LEN - 1] = '\0';

        char *token = strtok(copy, ","); // RunId
        token = strtok(NULL, ",");       // Username

        if (token && strcmp(token, username) == 0) {
            runs[index] = (char *) malloc(strlen(line) + 1);
            char *aux = (char *) malloc(strlen(line) + 1);
            strcpy(aux, line);

            strcpy(aux + strlen(aux) - 1, line + strlen(aux));

            strcpy(runs[index], aux);
            index++;
            free(aux);
        }
    }

    fclose(file);
    *countOut = count;
    return runs;
}


int main()
{
    init();

    char *userName = (char *) malloc(20);
    scanf("%s", userName);

    currentUser = createUser(userName);
    free(userName);


    FILE *users = fopen("users.csv", "r");
    FILE *runs = fopen("runs.csv", "r");
    
    char line[1000];
    if(fgets(line, sizeof(line), users) == NULL)
        createUsersCsv();

    strcpy(line, "");

    if(fgets(line, sizeof(line), runs) == NULL)
        createRunsCsv();
    


    if(checkUserNameInFile(currentUser->name) == -1)
    {
        appendUser(currentUser->name);
    }

    fclose(users);
    fclose(runs);

    
    menu(2, options0, header0);
    return 0;
}