#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Constants
int const TRUE = 1;
int const FALSE = 0;
int debugMode = 0;

//Both Struct Definitions
typedef struct mazeStruct{
	char **arr;  		//for a maze of any size
	int **posVisited;		//tracks positions visited
	int xsize, ysize; 	//tracks size of maze
	int xstart, ystart; //tracks starting positions
	int xend, yend; 	//tracks ending positions
} maze;

typedef struct stackStruct{
	int xpos, ypos;				//tracks current position in stack
	struct stackStruct *next;	//pointer to next node
}stack;

//Function Definitions
void readFile(FILE *src, int *xsize, int *ysize, int *xstart, int *ystart, int *xend, int*yend){

	while(fscanf (src, "%d %d", &*xsize, &*ysize) != EOF){
		if(*xsize > 0 && *ysize > 0)
			break;
		else{
			printf("Invalid Maze Size: %d, %d\n", *xsize, *ysize);
			*xsize = -1;
			*ysize = -1;
		}
	}
	while(fscanf (src, "%d %d", &*xstart, &*ystart) != EOF){
		if(*xstart <= *xsize && *ystart <= *ysize)
					break;
				else{
					printf("Invalid Maze Start Positions: %d, %d\n", *xstart, *ystart);
			*xstart = -1;
			*ystart = -1;
		}
	}
	while(fscanf (src, "%d %d", &*xend, &*yend) != EOF){
		if(*xend <= *xsize && *yend <= *ysize)
		break;
		else{
			printf("Invalid Maze End Positions: %d, %d\n", *xend, *yend);
			*xend = -1;
			*yend = -1;
		}
	}
}//end readFile()

void checkValidInputs(int xsize, int ysize, int xstart, int ystart, int xend, int yend){

	if((xsize == -1 || ysize == -1) || (xstart == -1 || ystart == -1)){
		printf("\nInvalid Input: Standard Error\n");
		exit(1);
	}
	if(xend == -1 || yend == -1){
		printf("\nReached end of file without finding valid size, start, and end\n");
		exit(1);
	}
}//end checkValid()

void printMazeSize(int xsize, int ysize, int xstart, int ystart, int xend, int yend){
	printf ("\nsize: %d, %d\n", xsize, ysize);
	printf ("start: %d, %d\n", xstart, ystart);
	printf ("end: %d, %d\n\n", xend, yend);
}//end printMazeSize()

void initializeMaze(maze *maze, int xsize, int ysize, int xstart, int ystart, int xend, int yend){
	int i, j;

	//initialize maze size, start, and end positions
	maze->xsize = xsize;
	maze->ysize = ysize;
	maze->xstart = xstart;
	maze->ystart = ystart;
	maze->xend = xend;
	maze->yend = yend;

	//Dynamically allocate array and visited places
	maze->arr = (char **) malloc(sizeof(char *) * (xsize + 2));
	maze->posVisited = (int **) malloc(sizeof(int *) * (xsize + 2));
	for(i = 0; i < xsize + 2; i++){
		maze->arr[i] = (char *) malloc(sizeof(char) * (ysize + 2));
		maze->posVisited[i] = (int *) malloc(sizeof(int) * (ysize + 2));
	}

	// mark all places as unvisited
	for (i = 0; i < maze->xsize + 2; i++){
		for (j = 0; j < maze->ysize + 2; j++){
			maze->arr[i][j] = '.';
			maze->posVisited[i][j] = 0;
		}
	}

	//populate the maze
	for (i = 0; i < maze->ysize + 2; i++){
		maze->arr[0][i] = '*';
		maze->arr[maze->xsize + 1][i] = '*';
	maze->posVisited[0][i] = 1; // for the 2d array
	maze->posVisited[maze->xsize + 1][i] = 1;
	}
	for (i=  0; i < maze->xsize + 2; i++){
		maze->arr[i][0] = '*';
		maze->arr[i][maze->ysize + 1] = '*';
	maze->posVisited[i][0] = 1; // for the 2d array
	maze->posVisited[i][maze->ysize + 1] = 1;
	}

	maze->arr[maze->xstart][maze->ystart] = 's';
	maze->arr[maze->xend][maze->yend] = 'e';


	maze->posVisited[maze->xstart][maze->ystart] = 1;
}//end initializeMaze()

void blockedPositions(FILE *src, maze *maze, int *xpos, int *ypos, int xstart, int ystart, int xend, int yend, int xsize, int ysize){

	while (fscanf (src, "%d %d", &*xpos, &*ypos) != EOF){
 		if((*xpos == xstart && *ypos == ystart)|| (*xpos == xend && *ypos == yend)){
			printf("Invalid Position of Start or End: %d, %d\n", *xpos, *ypos);
			continue;
		}
		if(*xpos > xsize || *ypos > ysize){
			printf("Invalid Position Outside of Maze: %d, %d\n", *xpos, *ypos);
			continue;
		}
		if(*xpos < 1 || *ypos < 1){
			printf("Invalid position Of Maze: %d, %d\n", *xpos, *ypos);
			continue;
		}
		maze->arr[*xpos][*ypos] = '*';
		maze->posVisited[*xpos][*ypos] = 1;
	}
}//end blockedPositions()

void printMaze(maze *maze){
	int i, j;

	for (i = 0; i < maze->xsize + 2; i++){
		for (j = 0; j < maze->ysize + 2; j++)
			printf ("%c", maze->arr[i][j]);
		printf("\t");
		if(debugMode == FALSE){
			printf("\n");
		}
		if(debugMode == TRUE){
			for (j = 0; j < maze->ysize + 2; j++){
				printf ("%d", maze->posVisited[i][j]);
			}
			printf("\n");
		}
	}
}//end printMaze()

void push(stack **head, int xpos, int ypos){

	stack *temp = (stack *) malloc(sizeof(stack));
	temp->ypos = ypos;
	temp->xpos = xpos;
	temp->next = *head;

	*head = temp;
}//end push()

void pop(stack **head, int debugMode){

	stack *temp = *head;
	if(debugMode == TRUE)
		printf("Popping off stack: (%d, %d)\n", temp->xpos, temp->ypos);
	//if list Empty
	if(*head != NULL){
		//remove from list
		*head = (*head)->next;
		free(temp);
		return;
	}
	//empty
	printf("\nStack is empty\n");
	return;
}//end pop()
stack* top(stack *hd)
{
 return (hd);
}

int checkEmpty(stack *head){
	if(head == NULL)
		return 1;
	return 0;
}//end checkEmpty()

void printStack(stack *head){
	if(head->next == NULL){
    printf("(%d, %d) \n", head->xpos,head->ypos);
    return;
  }
  printStack(head->next);
  printf("(%d, %d) \n", head->xpos,head->ypos);
  return;
}//end printStack()

void resetStack(stack *head){

	free(head); //frees old stack

}//end resetStack()

void freeMaze(maze *maze){
	int i;
	for(i = 0; i < maze->xsize + 2; i++){
		free(maze->arr[i]);
		free(maze->posVisited[i]);
	}
	free(maze->arr);
	free(maze->posVisited);
}//end freeMaze()


int main (int argc, char **argv){
	//Variables for maze
	int xpos, ypos;
	int xsize =-1, ysize =-1;
	int xstart =-1, ystart =-1;
	int xend =-1, yend =-1;

	char **debugInput; //to hold debugvalue

//Initialize stack and maze
	stack *head = NULL;
	maze m1;

//for file scanf
	FILE *src;
	int i, j;
	//debug mode
	for(i = 0; i < argc; i++){
		if(strcmp(argv[i], "-d") == 0){
			printf("\nDebug Mode\n");
			debugMode = TRUE;
		}
	}


	/* verify the proper number of command line arguments were given */
	if(argc < 2) {
		printf("Usage: %s <input file name>\n", argv[0]);
		exit(-1);
	}

	/* Try to open the input file. */
	if ( ( src = fopen( argv[1], "r" )) == NULL ){
		printf ( "Can't open input file: %s\n", argv[1] );
		exit(-1);
	}

	// Read the files and check for valid inputs
	readFile(src, &xsize, &ysize, &xstart, &ystart, &xend, &yend);
	checkValidInputs(xsize, ysize, xstart, ystart, xend, yend);

	printMazeSize(xsize, ysize, xstart, ystart, xend, yend);

	initializeMaze(&m1, xsize, ysize, xstart, ystart, xend, yend);
  blockedPositions(src, &m1, &xpos, &ypos, xstart, ystart, xend, yend, xsize, ysize);

	//print the maze
	printf("Maze\n\n");
	printMaze(&m1);

	// push to stack
	push(&head, xstart, ystart);

	//for current positions
	int xcurrent = xstart;
	int ycurrent = ystart;

	while(checkEmpty(head) != 1){
		// top element
		if(head->xpos == m1.xend && head->ypos == m1.yend)
			break;

		// right
		if(m1.posVisited[xcurrent][ycurrent + 1] == 0){
			ycurrent++;
			push(&head, xcurrent, ycurrent);
			//mark the unvisited side
			m1.posVisited[xcurrent][ycurrent] = 1;
			m1.arr[xcurrent][ycurrent] = 'o';
		}
		// down
		else if(m1.posVisited[xcurrent + 1][ycurrent] == 0){
			xcurrent++;
			push(&head, xcurrent, ycurrent);
						m1.posVisited[xcurrent][ycurrent] = 1;
						m1.arr[xcurrent][ycurrent] = 'o';
		}
		// left
		else if(m1.posVisited[xcurrent][ycurrent - 1] == 0){
			ycurrent--;
			push(&head, xcurrent, ycurrent);
						m1.posVisited[xcurrent][ycurrent] = 1;
						m1.arr[xcurrent][ycurrent] = 'o';
		}
		// up
		else if(m1.posVisited[xcurrent - 1][ycurrent] == 0){
			xcurrent--;
			push(&head, xcurrent, ycurrent);
						m1.posVisited[xcurrent][ycurrent] = 1;
						m1.arr[xcurrent][ycurrent] = 'o';
		}

		else{
			pop(&head, debugMode);
			if(checkEmpty(head)) //check for no solution found
				continue;
			xcurrent = head->xpos;
			ycurrent = head->ypos;
		}
	}

	//if stack is empty
	if(checkEmpty(head)){
		printf("\nMaze has no Solution\n\n");
		exit(1);
	}
	else{ //the items on the stack contain the coordinates of the solution from the end of the maze to start of the maze
		printf("\n\nSolution \n");
		printMaze(&m1);
		printf("\n");
		printf("Cordinates of the maze in reverse order\n\n");
		printStack(head);
		exit(1);
	}

	//deallocate memory
	freeMaze(&m1);

}//end main
