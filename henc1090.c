/*
 KEDAR PANDHARE  cs610 prp 1090
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* there are 256 possible characters */

#define NUM_CHARS	256

//defining tree node & heap node 

typedef struct _treenode treenode;
struct _treenode {
	int		frequency;	/* frequency; is the priority for heap */
	unsigned char	ch;	/* character, if any */
	treenode	*left,	/* left child of Huffman tree (not heap!) */
			*right;	/* right child of Huffman tree */
};

/* this represents  a priority queue implemented as a binary heap */
typedef struct _pq {
	int		heap_size;
	treenode	*A[NUM_CHARS];
} PQ;

/* create an empty queue */

void create_pq (PQ *p) {
	p->heap_size = 0;
}

/* this  represents heap node's parent */

int parent (int i) {
	return (i-1) / 2;
}

/* this represents heap node's left kid */

int left (int i) {
	return i * 2 + 1;
}

/* this represents heap node's right kid */

int right (int i) {
	return i * 2 + 2;
}


void Heapify (PQ *p, int i) {
	int		l, r, smallest;
	treenode	*t;

	l = left (i);
	r = right (i);

	/* finding the smallest amongst parent, left, and right */

	if (l < p->heap_size && p->A[l]->frequency < p->A[i]->frequency) 
		smallest = l;
	else
		smallest = i;
	if (r < p->heap_size && p->A[r]->frequency < p->A[smallest]->frequency)
		smallest = r;



	if (smallest != i) {
		t = p->A[i];
		p->A[i] = p->A[smallest];
		p->A[smallest] = t;
		Heapify (p, smallest);
	}
}

// code that inserts an element into the priority queue.
void insert_pq (PQ *p, treenode *r) {
	int		i;

	p->heap_size++;
	i = p->heap_size - 1;

	
	 
	while ((i > 0) && (p->A[parent(i)]->frequency > r->frequency)) {
		p->A[i] = p->A[parent(i)];
		i = parent (i);
	}
	p->A[i] = r;
}

/* remove the element (i.e., with minimum frequency) */
treenode *extract_min_pq (PQ *p) {
	treenode	*r;
	
	if (p->heap_size == 0) {
		printf ("heap underflow!\n");
		exit (1);
	}

	/* get return value out of the root */

	r = p->A[0];

	/* take the last and stick it in the root (just like heapsort) */

	p->A[0] = p->A[p->heap_size-1];

	/* one less thing in queue */

	p->heap_size--;

	/* left and right are a heap, make the root a heap */

	Heapify (p, 0);
	return r;
}

/* read the file, computing the frequencies for each character
 * and placing them in v[]
 */
unsigned int get_frequencies (FILE *f, unsigned int v[]) {
	int	r, n;

	/* n is used to count  characters */

	for (n=0;;n++) {

		

		r = fgetc (f);
	
		// get out of loop if no more characters are found 

		if (feof (f)) break;

		

		v[r]++;
	}
	return n;
}

/* code that generates Huffman tree (Huffman's Algorithm) */

treenode *build_huffman (unsigned int freqs[]) {
	int		i, n;
	treenode	*x, *y, *z;
	PQ		p;

	/* create an empty queue */

	create_pq (&p);

	/* for each character, make a heap/tree node with its value
	 * and frequency 
	 */
	for (i=0; i<NUM_CHARS; i++) {
		x = malloc (sizeof (treenode));

		/* its a leaf of the Huffman tree */

		x->left = NULL;
		x->right = NULL;
		x->frequency = freqs[i];
		x->ch = (char) i;

		/* put this node into the heap */

		insert_pq (&p, x);
	}


	n = p.heap_size-1; 

	
	for (i=0; i<n; i++) {

		/* create  new node z from the two least frequent
		 * nodes x and y
		 */
		z = malloc (sizeof (treenode));
		x = extract_min_pq (&p);
		y = extract_min_pq (&p);
		z->left = x;
		z->right = y;

		/* z's frequency is the sum of x and y */

		z->frequency = x->frequency + y->frequency;

		/* insert it  back in the queue */

		insert_pq (&p, z);
	}


	return extract_min_pq (&p);
}

// traverse the Huffman tree

void traverse (treenode *r, 	//root 
		int level, 	/* current level in Huffman tree */
		char code_so_far[], 
		char *codes[]) {



	if ((r->left == NULL) && (r->right == NULL)) {

		

		code_so_far[level] = 0;

		/* make a copy of the code and put it in the array */

		codes[r->ch] = strdup (code_so_far);
	} else {

		

		code_so_far[level] = '0';
		traverse (r->left, level+1, code_so_far, codes);

		

		code_so_far[level] = '1';
		traverse (r->right, level+1, code_so_far, codes);
	}
}


int nbits, current_byte, nbytes;

/* output a single bit to an open file */

void bitout (FILE *f, char b) {

	/* left shift current byte by one */

	current_byte <<= 1;



	if (b == '1') current_byte |= 1;

	

	nbits++;

	
	if (nbits == 8) {
		fputc (current_byte, f);
		nbytes++;
		nbits = 0;
		current_byte = 0;
	}
}

//Encoding the File
void encode_file (FILE *infile, FILE *outfile, char *codes[]) {
	unsigned char ch;
	char	*s;

	

	current_byte = 0;
	nbits = 0;
	nbytes = 0;

	/* continue until end of file */

	for (;;) {

		/* get a char */

		ch = fgetc (infile);
		if (feof (infile)) break;

		/* insert  the  bitstring on outfile */

		for (s=codes[ch]; *s; s++) bitout (outfile, *s);
	}

	/* finish off the last byte */

	while (nbits) bitout (outfile, '0');
}

// main program 
	
int main (int argc, char *argv[]) {
	FILE		*f, *g;
	treenode	*r;		   //root
	unsigned int	n, 		   /* number of bytes in file */
			freqs[NUM_CHARS];  /* frequency of each char */
	char		*codes[NUM_CHARS], /* array of codes, 1 per char */
			code[NUM_CHARS],   /* a place to hold one code */
			fname[100];	   /* what to call output file */



	if (argc != 2) {
		fprintf (stderr, "Usage: %s <filename>\n", argv[0]);
		exit (1);
	}

	/* seting all the  frequencies to zero */

	memset (freqs, 0, sizeof (freqs));

	/* opening command line argument file */

	f = fopen (argv[1], "r");
	if (!f) {
		perror (argv[1]);
		exit (1);
	}

	//compute frequencies 

	n = get_frequencies (f, freqs);
	fclose (f);

	//generate the huffman tree 

	r = build_huffman (freqs);

	

	traverse (r, 0, code, codes);

	/* naming the output file as .huf */

	sprintf (fname, "%s.huf", argv[1]);
	g = fopen (fname, "w");
	if (!g) {
		perror (fname);
		exit (1);
	}

	

	fwrite (freqs, NUM_CHARS, sizeof (int), g);



	fwrite (&n, 1, sizeof (int), g);

	

	f = fopen (argv[1], "r");
	if (!f) {
		perror (argv[1]);
		exit (1);
	}


	encode_file (f, g, codes);
	fclose (f);
	fclose (g);
		printf ("%s is %0.2f%% of %s\n", 
		fname, (float) nbytes / (float) n, argv[1]);
	exit (0);
}