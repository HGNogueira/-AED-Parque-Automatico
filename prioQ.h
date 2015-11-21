
/******************************************************************************
 *
 * File Name: prioQ.h
 *	      (c) 2010 AED
 * Author:    AED team
 * Revision:  v2.3 ACR
 * NAME
 *     prioQ.h - prototypes
 *
 * SYNOPSIS
 *     #include <stdlib.h>
 *     #include <stdio.h>
 *     #include <string.h>
 *
 * DESCRIPTION
 *
 * DIAGNOSTICS
 *
 *****************************************************************************/

typedef struct _prioQ PrioQ;

PrioQ *NewPrioQ(int size);

void FreeprioQ(PrioQ * h);

int Insert(PrioQ * h, int element);

int Direct_Insert(PrioQ * h, int element);

void Modify(PrioQ * h, int indice, int newvalue);

int RemoveMax(PrioQ * h);

int PrintprioQ(PrioQ * h);

void CleanprioQ(PrioQ * h);

int VerifyprioQ(PrioQ * h);

void prioQSort(PrioQ * h);

int prioQify(PrioQ * h);
