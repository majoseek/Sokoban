#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<assert.h>
#include<limits.h>


#define UP 8
#define LEFT 4
#define RIGHT 6
#define DOWN 2
#define MULTIPLIER 2


struct area
{
    bool target_field;
    bool visited;
    char val;
};
typedef struct area Tarea;
struct chest
{
    int x;
    int y;
    char letter;
};
typedef struct chest Tchest;

struct previous_move
{
    char chest_letter;
    int player_x;
    int player_y;
};
typedef struct previous_move Tprevious_move;
struct stack
{
    int top;
    Tprevious_move *moves;
};
typedef struct stack Tstack;

bool empty_stack(Tstack *S)
{
    if(S->top==0)
        return true;
    return false;
}
void initStack(Tstack *S)
{
    S->top=0;
    S->moves=malloc(sizeof(Tprevious_move));
}
void pushStack(Tstack *S,int playerx,int playery,char chestletter)
{
    S->moves[S->top].player_x=playerx;
    S->moves[S->top].player_y=playery;
    S->moves[S->top].chest_letter=chestletter;
    (S->top)++;
}
Tprevious_move popStack(Tstack *S)
{
    (S->top)--;
    return S->moves[S->top];
}
void clearStack(Tstack *S)
{
    S->top=0;
    free(S->moves);
}
bool is_free(char c)
{
    if(c=='+' || c=='-' || c=='@')
        return true;
    else
        return false;
}

char to_upper(char c)
{
    return c-32;
}

char to_lower(char c)
{
    return c+32;
}

void get_board(Tarea ***board,Tchest chests[27],int *chests_count,int *row_count,int **columns_count,int *player_x,int *player_y)
{
    bool newline=true,endinput=false;
    int c=0;
    int row=0,column=0,row_size=0,column_size=0;
    *board=(Tarea**)malloc(sizeof(Tarea*));
    *columns_count=malloc(sizeof(int));

    while(!endinput && (c=getchar())!=EOF)
    {
        assert(row<INT_MAX);
        assert(column<INT_MAX);
        if(row==row_size)
        {
            row_size=1+row_size*MULTIPLIER;
            *columns_count=realloc(*columns_count,row_size*sizeof(int*));
            *board=(Tarea**)realloc(*board,row_size * sizeof (Tarea*));
            (*board)[row]=(Tarea*) malloc(sizeof(Tarea));
            assert(*board!=NULL);
        }

        if(c=='\n')
        {
            if(newline==true)
                endinput=true;
            
            (*columns_count)[row]=column;
            row++;
            column=0;
            column_size=0;
            newline=true;
        }
        else
        {
            newline=false;
            if(column==column_size)
            {
                column_size=1+column_size*MULTIPLIER;
                (*board)[row]=(Tarea*) realloc((*board)[row],column_size * sizeof (Tarea));
                assert((*board)[row]!=NULL);
            }

            (*board)[row][column].val=c;
            (*board)[row][column].visited=false;
            (*board)[row][column].target_field=false;

            if(c=='*' || (c>='A' && c<='Z') || c=='+')
                (*board)[row][column].target_field=true;

            if(c!='#')
            {
                if(c>='a' && c<='z')
                {
                    chests[*chests_count].letter=c;
                    chests[*chests_count].x=row;
                    chests[*chests_count].y=column;
                    (*chests_count)++;
                }
                else if (c>='A' && c<='Z')
                {
                    chests[*chests_count].letter=to_lower(c);
                    chests[*chests_count].x=row;
                    chests[*chests_count].y=column;
                    (*chests_count)++;
                }
                else if(c=='@' || c=='*')
                {
                    *player_x=row;
                    *player_y=column;
                }
            }

            column++;
        }

    }
    *row_count=row-1;
}

void empty_visited(Tarea ***board,int row_count,int *columns_count)
{
    for(int i=0;i<row_count;i++)
        for(int j=0;j<columns_count[i];j++)
            (*board)[i][j].visited=false;
}
Tchest find_chest(Tchest chests[27],int chest_count,char chest_letter)
{
    for(int i=0;i<chest_count;i++)
        if(chests[i].letter==chest_letter)
            return chests[i];

    return chests[0];
}
bool can_move(int row_count,int *columns_count,int player_x,int player_y,int dir)
{

    if(dir==UP)
    {
        if(player_x==0)
            return false;
        else if(columns_count[player_x-1]<player_y)
            return false;
        return true;
    }
    else if(dir==RIGHT)
    {
        if(player_y==columns_count[player_x]-1)
            return false;
        return true;
    }
    else if(dir==LEFT)
    {
        if(player_y==0)
            return false;
        return true;
    }
    else if(dir==DOWN)
    {
        if(player_x==row_count-1)
            return false;
        else if(columns_count[player_x+1]<player_y)
            return false;
        return true;
    }

    return false;
}
bool push_box(Tarea **board,Tchest chest,int dir,int row_count,int *columns_count)
{
    int chest_x,chest_y;
            chest_x=chest.x;
            chest_y=chest.y;

            if(dir==UP)
            {
                if(chest_x==0)
                    return false;
                else if(columns_count[chest_x-1]<chest_y)
                    return false;
                else if(!is_free(board[chest_x-1][chest_y].val))
                    return false;
                return true;
            }
            else if(dir==RIGHT)
            {
                if(chest_y==columns_count[chest_x]-1)
                    return false;
                else if(!is_free(board[chest_x][chest_y+1].val))
                    return false;
                return true;
            }
            else if(dir==LEFT)
            {
                if(chest_y==0)
                    return false;
                else if(!is_free(board[chest_x][chest_y-1].val))
                    return false;
                return true;
            }
            else if(dir==DOWN)
            {
                if(chest_x==row_count-1)
                    return false;
                else if(columns_count[chest_x+1]<chest_y)
                    return false;
                else if(!is_free(board[chest_x+1][chest_y].val))
                    return false;
                return true;
            }

    return false;
}
void print_board(Tarea **board,int row_count,int *columns_count)
{
    for(int i=0;i<row_count;i++)
    {
        for(int j=0;j<columns_count[i];j++)
            putchar(board[i][j].val);
        putchar('\n');
    }
}
void move_back(Tstack *S,Tchest chests[27],Tarea ***board,int chest_count,int *player_x,int *player_y)
{

    if(empty_stack(S))
        return;

    Tprevious_move move=popStack(S);
    bool found=false;
    int index,chestx,chesty;

    for(int i=0;i<chest_count && !found;i++)
    {
        if(chests[i].letter==move.chest_letter)
        {
            index=i;
            chestx=chests[i].x;
            chesty=chests[i].y;
            found=true;
        }
    }

    if((*board)[*player_x][*player_y].val=='@')
        (*board)[*player_x][*player_y].val='-';
    else
        (*board)[*player_x][*player_y].val='+';

     if((*board)[chestx][chesty].val>='a' && (*board)[chestx][chesty].val<='z')
        (*board)[chestx][chesty].val='-';
     else
         (*board)[chestx][chesty].val='+';

     chests[index].x=*player_x;
     chests[index].y=*player_y;

     if((*board)[*player_x][*player_y].val=='-')
         (*board)[*player_x][*player_y].val=move.chest_letter;
     else
         (*board)[*player_x][*player_y].val=to_upper(move.chest_letter);

    *player_x=move.player_x;
    *player_y=move.player_y;

     if((*board)[*player_x][*player_y].val=='-')
         (*board)[*player_x][*player_y].val='@';
     else
         (*board)[*player_x][*player_y].val='*';


}

void move_chest(Tarea ***board,Tstack *S,Tchest chests[27],int *player_x,int *player_y,int dir,char chest_letter,int chest_count)
{
    int chest_x,chest_y;
    bool found=false;
    int index;

    for(int i=0;i<chest_count && !found;i++)
    {
        if(chest_letter==chests[i].letter)
        {
            index=i;
            chest_x=chests[i].x;
            chest_y=chests[i].y;
            found=true;
        }
    }

    pushStack(S,*player_x,*player_y,chest_letter);

    if((*board)[*player_x][*player_y].val=='@')
        (*board)[*player_x][*player_y].val='-';
    else
        (*board)[*player_x][*player_y].val='+';

    if((*board)[chest_x][chest_y].val>='a' && (*board)[chest_x][chest_y].val<='z')
        (*board)[chest_x][chest_y].val='@';
    else
        (*board)[chest_x][chest_y].val='*';


    *player_x=chest_x;
    *player_y=chest_y;

    if(dir==UP)
    {
        chests[index].x--;
        chest_x--;
        if((*board)[chest_x][chest_y].val=='-' || (*board)[chest_x][chest_y].val=='@')
        {
            (*board)[chest_x][chest_y].val=chest_letter;
        }
        else if((*board)[chest_x][chest_y].val=='+' || (*board)[chest_x][chest_y].val=='*')
        {
            (*board)[chest_x][chest_y].val=to_upper(chest_letter);
        }
    }
    else if(dir==DOWN)
    {
        chest_x++;
        chests[index].x++;
        if((*board)[chest_x][chest_y].val=='-' || (*board)[chest_x][chest_y].val=='@')
            (*board)[chest_x][chest_y].val=chest_letter;
        else if((*board)[chest_x][chest_y].val=='+' || (*board)[chest_x][chest_y].val=='*')
            (*board)[chest_x][chest_y].val=to_upper(chest_letter);
    }
    else if(dir==LEFT)
    {
        chest_y--;
        chests[index].y--;
        if((*board)[chest_x][chest_y].val=='-' || (*board)[chest_x][chest_y].val=='@')
            (*board)[chest_x][chest_y].val=chest_letter;
        else if((*board)[chest_x][chest_y].val=='+' || (*board)[chest_x][chest_y].val=='*')
            (*board)[chest_x][chest_y].val=to_upper(chest_letter);
    }
    else if(dir==RIGHT)
    {
        chest_y++;
        chests[index].y++;
        if((*board)[chest_x][chest_y].val=='-' || (*board)[chest_x][chest_y].val=='@')
            (*board)[chest_x][chest_y].val=chest_letter;
        else if((*board)[chest_x][chest_y].val=='+' || (*board)[chest_x][chest_y].val=='*')
            (*board)[chest_x][chest_y].val=to_upper(chest_letter);
    }
    
}
bool path_exists(Tarea ***board,Tchest chest,int player_x,int player_y,int row_count,int *columns_count,int dir)
{
    //printf("sprawdzam skrzynia x %d y %d gracz x %d y %d kierunek %d\n",chest.x,chest.y,player_x,player_y,dir);
    
    (*board)[player_x][player_y].visited=true;
    if(dir==UP && player_x==chest.x+1 && player_y==chest.y)
        return true;
    else if(dir==DOWN && player_x==chest.x-1 && player_y==chest.y)
        return true;
    else if(dir==LEFT && player_x==chest.x && player_y==chest.y+1)
        return true;
    else if(dir==RIGHT && player_x==chest.x && player_y==chest.y-1)
        return true;

    if(can_move(row_count,columns_count,player_x,player_y,DOWN))
        if((*board)[player_x+1][player_y].visited==false)
            if(is_free((*board)[player_x+1][player_y].val) && push_box(*board,chest,dir,row_count,columns_count))
                if(path_exists(board,chest,player_x+1,player_y,row_count,columns_count,dir))
                    return true;
    if(can_move(row_count,columns_count,player_x,player_y,UP))
        if((*board)[player_x-1][player_y].visited==false)
            if(is_free((*board)[player_x-1][player_y].val) && push_box(*board,chest,dir,row_count,columns_count))
                if(path_exists(board,chest,player_x-1,player_y,row_count,columns_count,dir))
                    return true;

    if(can_move(row_count,columns_count,player_x,player_y,RIGHT))
        if((*board)[player_x][player_y+1].visited==false)
            if(is_free((*board)[player_x][player_y+1].val) && push_box(*board,chest,dir,row_count,columns_count))
                if(path_exists(board,chest,player_x,player_y+1,row_count,columns_count,dir))
                    return true;

    if(can_move(row_count,columns_count,player_x,player_y,LEFT))
        if((*board)[player_x][player_y-1].visited==false)
            if(is_free((*board)[player_x][player_y-1].val) && push_box(*board,chest,dir,row_count,columns_count))
                if(path_exists(board,chest,player_x,player_y-1,row_count,columns_count,dir))
                    return true;



    return false;

}
void input_commands(Tarea ***board,Tstack *S,Tchest chests[27],int *columns_count,int row_count,int chest_count,int *player_x,int *player_y)
{

    print_board(*board,row_count,columns_count);
    int input=getchar();
    int input_2;
    while(input!=EOF && input!='.')
    {
        if(input>='a' && input<='z')
        {
            input_2=getchar();
            input_2-='0';
            if(!push_box(*board,find_chest(chests,chest_count,input),input_2,row_count,columns_count))
            {
                print_board(*board,row_count,columns_count);
            }
            else
            {
                if(path_exists(board,find_chest(chests,chest_count,input),*player_x,*player_y,row_count,columns_count,input_2))
                    move_chest(board,S,chests,player_x,player_y,input_2,input,chest_count);
                empty_visited(board,row_count,columns_count);
                print_board(*board,row_count,columns_count);                
            }
            
        }
        else if(input=='0')
        {
            move_back(S,chests,board,chest_count,player_x,player_y);
            print_board(*board,row_count,columns_count);                
        }
        
        input=getchar();
    }
}
void empty_chests(Tchest arr[27])
{
    for(int i=0;i<27;i++)
    {
        arr[i].letter=0;
        arr[i].x=0;
        arr[i].y=0;
    }
}
int main()
{
    Tarea **board=0;
    Tchest chests[27];
    empty_chests(chests);
    Tstack S;
    initStack(&S);
    int player_x=0,player_y=0;
    int *columns_count,row_count=0,chest_count=0;
    get_board(&board,chests,&chest_count,&row_count,&columns_count,&player_x,&player_y);
    input_commands(&board,&S,chests,columns_count,row_count,chest_count,&player_x,&player_y);

    for(int i=0;i<row_count;i++)
        free(board[i]);
    free(board);
    free(columns_count);
    clearStack(&S);

    return 0;
}