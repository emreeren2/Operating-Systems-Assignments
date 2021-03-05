// This code will simulate a simple Airline Reservation System which will involve accessing shared resources.
// We will use two travell agencies, which will be assigned to two different threads.

#include <iostream>
#include <pthread.h>
#include <stdlib.h>

using namespace std;

int Plane[2][50] = {0}; // creating 2*50 matrix, that filled with 0s
int capacity = 100;
int turn = 0;
bool contin = true;     // This variable is used for, both in main while loop contition, that placed in thread functions,
                        // and inside the one of the if statemnt, that prevents mistake, which will be explained below (with an example)
                        // When agency1 enters critical region and agency2 is in busy waiting while plane is full (capacity is 0),
                        // it should inform in final step as " agency1 enter crital region", "agency1 exit critical region",
                        // "no seats left" and "plane is full" one after another.
                        // in that step i change the "contin" variable as false, and this means that
                        // agency1 function is done its tasks and completely finished as not to be entered again.
                        //
                        // Since i change the turn at the end of the agency1 function,
                        // agency2 will enter critical region and it will try to inform that
                        // "agency2 enter critical region" and "agency2 exit critcal region",
                        // "no seats left" and "plane is full" stuffs one after another, which should not to do.
                        // To prevent this, we assigned "contin" variable as false in agency1 function,
                        // therefore it is not entered the function, which
                        // displays "agency2 enter critical region", "agency2 exit critcal region",
                        // "no seats left" and "plane is full" stuffs.
                        // Since "contin" is global variable, and we assign it as false in agency1 function,
                        // agency2 function cannot continue because it cannot enter main loop again and function will be finsihed.

// This function display entire matrix, which represents seats in plane in that case
void showPlane()
{
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 50; j++) {
      cout << Plane[i][j] << " ";
    }
    cout << endl;
  }
}

// This function converts randomly genrated number to seat coordinates.
void converter(int & row, int & col ,const int & seat)
{
  if (seat <= 50) {       // For seats that stay in the first row (actually right side seats)
    row = 0;              // (we work with first raw in that case)
    col = seat - 1;
  }
  else if (seat == 100) { // Since 100 % 50 = 0, i need to specify 100th seat
    row = 1;
    col = 49;
  }
  else{                   // For seats that stays in the second row (actually left side seats)
    row = 1;              // (we work with the second row in that case)
    col = (seat % 50) - 1;
  }
}


// This function is run by agency1 thread.
// it basically does generate random seat number, then check that seat is reserved or not,
// if it is not reserved, agency1 reserves that seat and marks that seat as 1 which means that,
// this seat is reserved by agency1.
// These step (excluding generating random number), which uses shared variable,
// is done while agency2 function is in busy-waiting
// Therefore we prevent overlapping.
void *reserverFirstAgency(void *)
{
  while (capacity != 0 && contin) {
    int agency1_rand_seat = rand() % 100 + 1; // Generating random number between 1 and 100

    while (turn != 0) {    } // Busy-Waiting

    int row, col;
    converter(row, col, agency1_rand_seat);

    if (Plane[row][col] == 0) {
      cout << "Agency 1 Entered Critical Region" <<endl;
      Plane[row][col] = 1; // set as 1, which means that agency 1 reserved that seat
      cout << "Seat Number " << agency1_rand_seat << " is reserved by Agency 1" <<endl;
      capacity--; // decrementing capacity
      cout << "Agency 1 Exit Critical Region" <<endl<<endl;
    }

    // i put it a control here because if agency2 takes last seat while agency1 is in busy waiting,
    // after turn changes, agency1 tries to find empty seat, which does not exist anymore
    // explanation above is relating with "capacity == 0" part
    if (capacity == 0 && contin) {  // ("contin" part is explained detaily where it is defined)
      cout << "Agency 1 Entered Critical Region" <<endl;
      cout << "Agency 1 Exit Critical Region" <<endl;
      cout << "No Seats Left" <<endl<<endl;
      cout << "Plane is full:" <<endl<<endl;
      contin = false; // so we will not enter while loop again and
    }                 // in agency2 function we will not enter that if statement,
                      // which is also placed inside the agency2 function.

    turn = 1; // to let continue agency2 function.
  }
}


//                Explanations are all same with above agency1 function.
// This function is run by agency2 thread.
// it basically does generate random seat number, then check that seat is reserved or not,
// if it is not reserved, agency2 reserves that seat and marks that seat as 2 which means that,
// this seat is reserved by agency2.
// These step (excluding generating random number), which uses shared variable,
// is done while agency1 function is in busy-waiting
// Therefore we prevent overlapping.
void *reserverSecondAgency(void *)
{
  while (capacity != 0 && contin) {
    int agency2_rand_seat = rand() % 100 + 1; // Generating random number between 1 and 100

    while (turn != 1) {    } // Busy-Waiting

    int row, col;
    converter(row, col, agency2_rand_seat);

    if (Plane[row][col] == 0) {
      cout << "Agency 2 Entered Critical Region" <<endl;
      Plane[row][col] = 2; // set as 2, which means that agency 2 reserved that seat
      cout << "Seat Number " << agency2_rand_seat << " is reserved by Agency 2" <<endl;
      capacity--; // decrementing capacity
      cout << "Agency 2 Exit Critical Region" <<endl<<endl;
    }

    // i put it a control here because if agency1 takes last seat while agency2 is in busy waiting,
    // after turn changes, agency2 tries to find empty seat, which does not exist anymore
    // explanation above is relating with "capacity == 0" part
    if (capacity == 0 && contin) { // ("contin" part is explained detaily where it is defined)
      cout << "Agency 2 Entered Critical Region" <<endl;
      cout << "Agency 2 Exit Critical Region" <<endl;
      cout << "No Seats Left" <<endl<<endl;
      cout << "Plane is full:" <<endl<<endl;

      contin = false; // so we will not enter while loop again and
    }                 // in agency2 function we will not enter that if statement,
                      // which is also placed inside the agency2 function.

    turn = 0; // to let continue agency1 function.
  }
}


int main()
{
  srand(time(NULL));
  pthread_t travelAgency1, travelAgency2;

  pthread_create(&travelAgency1, NULL, reserverFirstAgency, NULL);
  pthread_create(&travelAgency2, NULL, reserverSecondAgency, NULL);

  pthread_join(travelAgency1, NULL);
  pthread_join(travelAgency2, NULL);

  showPlane();

  return 0;
}
