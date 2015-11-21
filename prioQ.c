#include <stdlib.h>
#include <stdio.h>
#include "prioQ.h"

/* A prioQ is represented by a structure: */
struct _prioQ {
  int n_elements;               /* # elements in prioQ */
  int size;                     /* max size of the prioQ. */
  int *prioQdata;               /* indexed table of node values */
};


/******************************************************************************
 * PrintprioQ()
 *
 * Arguments: h - pointer to prioQ structure
 * Returns: void
 * Side-Effects: none
 *
 * Description: auxiliary function to print prioQ content
 *
 *****************************************************************************/

int PrintprioQ(PrioQ * h)
{
    int i;
    
    if (h->n_elements == 0) {
        printf("prioQ empty.\n");
        return 0;
    }
    printf("\n");
    for (i = 0; i < h->n_elements; i++)
        printf("+-----");
    printf("+\n");
    for (i = 0; i < h->n_elements; i++)
        printf("| %3d ", i);
    printf("|\n");
    for (i = 0; i < h->n_elements; i++)
        printf("+-----");
    printf("+\n");
    for (i = 0; i < h->n_elements; i++) {
        printf("| ");
        fprintf(stdout, "%d", h->prioQdata[i]);
        printf(" ");
    }
    printf("|\n");
    for (i = 0; i < h->n_elements; i++)
        printf("+-----");
    printf("+\n");
    
    return h->n_elements;
}


/******************************************************************************
 * FixUp()
 *
 * Arguments: h - pointer to prioQ structure
 *            k - index of element to fixup
 * Returns:
 * Side-Effects: none
 *
 * Description: Performs fixup from index
 *
 *****************************************************************************/

void FixUp(prioQ * h, int k)
{
  int t;
  while ((k > 0) && (h->prioQdata)[(k - 1) / 2] < (h->heapdata)[k]) {
    t = (h->prioQdata)[k];
    (h->prioQdata)[k] = (h->heapdata)[(k - 1) / 2];
    (h->prioQdata)[(k - 1) / 2] = t;

    k = (k - 1) / 2;
  }

  return;
}


/******************************************************************************
 * FixDown()
 *
 * Arguments: h - pointer to prioQ structure
 *            k - index of element to fixdown
 * Returns:
 * Side-Effects: none
 *
 * Description: performs fixdown from index
 *
 *****************************************************************************/

int FixDown(prioQ * h, int k)
{
  int j;
  int n_ops = 0;
  int t;

  while ((2 * k + 1) < h->n_elements) { 
    j = 2 * k + 1;
    n_ops++;
    if (((j + 1) < h->n_elements) &&
        h->prioQdata[j], h->heapdata[j + 1]) {   /* operation */
      /* second offspring is greater */
      j++;
    }
    n_ops++;
    if (! h->prioQdata[k] < h->heapdata[j]) {   /* operation */
      /* Elements are in correct order. */
      break;
    }

    /* the 2 elements are not correctly sorted, it is
       necessary to exchange them */
    t = (h->prioQdata)[k]; 
    (h->prioQdata)[k] = (h->heapdata)[j];
    (h->prioQdata)[j] = t;
    k = j;
    n_ops += 3;
  }
  n_ops++;

  return n_ops;
}


/******************************************************************************
 * NewprioQ()
 *
 * Arguments: size - prioQ size
 * Returns: prioQ
 * Side-Effects: none
 *
 * Description: allocates space for a new prioQ of specified size
 *
 *****************************************************************************/

prioQ *NewPrioQ(int size)
{
  prioQ *h;

  h = (prioQ *) malloc(sizeof(PrioQ));
  if (h == ((prioQ *) NULL)) {
    fprintf(stderr, "Error in malloc of prioQ\n");
    exit(1);
  }

  h->n_elements = 0;
  h->size = size;
  h->prioQdata = (int *) malloc(size * sizeof(int));
  if (h->prioQdata == NULL) {
    fprintf(stderr, "Error in malloc of prioQ data\n");
    exit(1);
  }

  return (h);
}

/******************************************************************************
 * NewprioQ()
 *
 * Arguments: size - prioQ size
 * Returns: prioQ
 * Side-Effects: none
 *
 * Description: allocates space for a new prioQ of specified size
 *
 *****************************************************************************/

void FreeprioQ(PrioQ *h)
{
    CleanprioQ(h);

    free(h->prioQdata);
    free(h);

    return;
}

/******************************************************************************
 * Insert()
 *
 * Arguments: h - pointer to prioQ
 *            element - pointer to new element
 * Returns: vopid
 * Side-Effects: none
 *
 * Description: add element at the end of prioQ and do fixup
 *
 *****************************************************************************/

int Insert(prioQ * h, int element)
{
  if (h->n_elements == h->size) {
    printf("prioQ full (size = %d) !\n", h->size);
    return 0;
  }
  h->prioQdata[h->n_elements] = element;

  h->n_elements++;
  FixUp(h, h->n_elements - 1);

  return 1;
}

/******************************************************************************
 * Direct_Insert()
 *
 * Arguments: h - pointer to prioQ
 *            element - pointer to new element
 * Returns: vopid
 * Side-Effects: none
 *
 * Description: add element at the end of prioQ but does not perform fixup
 *
 *****************************************************************************/

int Direct_Insert(prioQ * h, int element)
{
  if (h->n_elements == h->size) {
    printf("prioQ full (size = %d) !\n", h->size);
    return 0;
  }
  h->prioQdata[h->n_elements] = element;

  h->n_elements++;


  return 1;
}

/******************************************************************************
 * Modify()
 *
 * Arguments: h - pointer to prioQ
 *            index - index of element to modify
 *            newvalue - new element value
 * Returns: void
 * Side-Effects: none
 *
 * Description: compares new element with antecedent, if larger do a fixup
 *              otherwise do fixdown
 *
 *****************************************************************************/

void Modify(prioQ * h, int index, int newvalue)
{
  if (index > h->n_elements - 1) {
    printf("Index out of range (index = %d) !\n", index);
    return;
  }
  /* Compares new value  with the value of the element to substitute */
  if ( newvalue < h->prioQdata[index]) {
    /* If smaller, reconstruct prioQ with function FixDown */
    h->prioQdata[index] = newvalue;
    FixDown(h, index);
  }
  else {
    /* If greater, reconstruct prioQ using the function FixUp */
    h->prioQdata[index] = newvalue;
    FixUp(h, index);
  }

  return;
}


/******************************************************************************
 * RemoveMax()
 *
 * Arguments: h - pointer to prioQ
 * Returns: int
 * Side-Effects: none
 *
 * Description: exchange first and last element of prioQ, remove last element
 *              (max element) from prioQ and maintain heap order by performing
 *              a fixdown of the first element
 *
 *****************************************************************************/

int RemoveMax(prioQ * h)
{
  int t;

  if (h->n_elements > 0) {
    t = (h->prioQdata)[0];
    (h->prioQdata)[0] = (h->heapdata)[h->n_elements - 1];
    (h->prioQdata)[h->n_elements - 1] = t;
    h->n_elements--;
    FixDown(h, 0);
    return t;
  }

  return -1;
}

/******************************************************************************
 * CleanprioQ()
 *
 * Arguments: prioQ
 * Returns: none
 * Side-Effects: None
 *
 * Description: cleans an prioQ
 *
 *****************************************************************************/

void CleanprioQ(PrioQ * h)
{
    int i = 0;

    for(i = 0; i < h->n_elements; i++) 
        h->prioQdata[i] = 0;
    h->n_elements = 0;
    
    return;
}

/******************************************************************************
 * VerifyprioQ()
 *
 * Arguments: prioQ
 * Returns: int (1 if it is an prioQ; 0 otherwise)
 * Side-Effects: None
 *
 * Description: verifies if the argument satisfies the prioQ condition
 *
 *****************************************************************************/

int VerifyprioQ(PrioQ * h)
{
    int i;

    for(i = h->n_elements - 1; i > 0 ; i--){
        if(!(h->prioQdata[i] < h->heapdata[(i-1)/2]))
            return 0;
    }
    
    return 1;
}

/******************************************************************************
 * prioQSort()
 *
 * Arguments: prioQ
 * Returns: void
 * Side-Effects: 
 *
 * Description: sorts an prioQ using the heapsort algorithm
 *
 *****************************************************************************/

void prioQSort(PrioQ * h)
{
    int i = 0;
    int auxN;
    int n_ops = 0;;
    int t;

    n_ops += prioQify(h);  /* operation */
    auxN = h->n_elements;

    for(i = 0; i < auxN; i++){  /* operation */
        t = h->prioQdata[0]; /* operation */
        h->prioQdata[0] = h->heapdata[h->n_elements - 1]; /* operation */
        h->prioQdata[h->n_elements - 1] = t;  /* operation */
        h->n_elements--;
        n_ops += 4;
        n_ops += FixDown(h, 0);  /* operation */
    }
    n_ops++;
        
    h->n_elements = auxN;
    PrintprioQ(h);

    fprintf(stdout, "[prioQ SORT] total number of ops: %d\n", n_ops);

    return;
}

/******************************************************************************
 * prioQify()
 *
 * Arguments: prioQ
 * Returns: void
 * Side-Effects:
 *
 * Description: forces the argument to be an prioQ
 *
 *****************************************************************************/

int prioQify(PrioQ * h)
{
    int  i;
    int n_ops = 0;

    for(i = (h->n_elements - 1)/2; i >= 0; i--)
        n_ops +=FixDown(h, i) + 1;
    n_ops++;

    PrintprioQ(h);
    
    return n_ops;
}

int isEmpty(PrioQ *h){
    return (h->n_elements > 0) ? 0 : 1;
}
