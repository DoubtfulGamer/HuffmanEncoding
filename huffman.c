#include <stdio.h>
#include <stdlib.h>

// Node structure for the Huffman tree
struct TNode {
  char c;           // Character stored in the node
  struct TNode *l, *r;  // Left and right child pointers
};

// Node structure for the linked list of trees (used during Huffman tree construction)
struct LNode {
  struct TNode *tree;  // Pointer to a tree node
  int f;               // Frequency of the character(s) in this tree
  struct LNode *next;  // Pointer to next node in linked list
};

// Creates a new tree node with character c
struct TNode* createTNode(char c) {
  struct TNode *t = malloc(sizeof(struct TNode));
  t -> c = c;
  t -> l = t -> r = NULL;
  return t;
}

// Creates a new linked list node containing a tree and its frequency
struct LNode* createLNode(struct TNode *t, int f) {
  struct LNode *n = malloc(sizeof(struct LNode));
  n -> f = f;
  n -> tree = t;
  n -> next = NULL;
  return n;
}

// Inserts a new tree node into the linked list in sorted order by frequency
struct LNode* addLNode(struct LNode *h, struct TNode *t, int f) {
  struct LNode *l = NULL, *n = NULL;

  if (h == NULL)
    return createLNode(t, f);

  if (f < h -> f) {
    n = createLNode(t, f);
    n -> next = h;
    return n;
  }

  for (l = h; (l -> next != NULL) && (l -> next -> f <= f); l = l -> next);

  n = createLNode(t, f);
  n -> next = l -> next;
  l -> next = n;
  return h;
}

// Counts character frequencies in the input string and creates a sorted linked list
struct LNode* createList(char str[]) {
  struct LNode *n = NULL;
  int freq[65] = {}, i = 0;

  // Count frequencies for digits (0-9), letters (A-Z, a-z), and special chars (space, ., ,)
  for (i = 0; str[i]; i++) {
    if ('0' <= str[i] && str[i] <= '9')
      freq[str[i] - '0']++;
    else if ('A' <= str[i] && str[i] <= 'Z')
      freq[str[i] - 'A' + 10]++;
    else if ('a' <= str[i] && str[i] <= 'z')
      freq[str[i] - 'a' + 36]++;
    else if (str[i] == ' ')
      freq[62]++;
    else if (str[i] == '.')
      freq[63]++;
    else if (str[i] == ',')
      freq[64]++;
    else {
      printf("ERROR: Invalid characters present in string!!\n");
      return NULL;
    }
  }

  // Build linked list of nodes with frequencies > 0, sorted by frequency
  for (i = 0; i < 65; i++) {
    if (freq[i] > 0)
      if (i < 10)
        n = addLNode(n, createTNode(i + '0'), freq[i]);
      else if (i < 36)
        n = addLNode(n, createTNode(i + 'A' - 10), freq[i]);
      else if (i < 62)
        n = addLNode(n, createTNode(i + 'a' - 36), freq[i]);
      else if (i == 62)
        n = addLNode(n, createTNode(' '), freq[i]);
      else if (i == 63)
        n = addLNode(n, createTNode('.'), freq[i]);
      else if (i == 64)
        n = addLNode(n, createTNode(','), freq[i]);
  }

  return n;
}

// Merges two tree nodes into a new parent node
struct TNode* mergeTree(struct TNode *a, struct TNode *b) {
  struct TNode *t = createTNode(0);

  t -> l = a;
  t -> r = b;

  return t;
}

// Merges the two lowest frequency trees and re-inserts into the linked list
struct LNode* mergeList(struct LNode *h) {
  struct TNode *temp = NULL;
  struct LNode *a = NULL, *b = NULL;

  if (h == NULL)
    return NULL;

  a = h;
  b = a -> next;

  temp = mergeTree(a -> tree, b -> tree);

  h = addLNode(h, temp, a -> f + b -> f);
  h = b -> next;

  free(a);
  free(b);

  return h;
}

// Recursively finds the Huffman code for a character and stores it in enc_str
int encode(struct TNode *tree, char ch, char *enc_str, int i) {
  if (tree -> c == ch) {
    enc_str[i] = '\0';
    return 1;
  }

  if (tree -> c != 0)
    return 0;

  enc_str[i] = '0';
  if (encode(tree -> l, ch, enc_str, i + 1))
    return 1;

  enc_str[i] = '1';
  if (encode(tree -> r, ch, enc_str, i + 1))
    return 1;

  enc_str[i] = '\0';
  return 0;
}

// Decodes a binary string using the Huffman tree and prints the original text
void decode(struct TNode *tree, char *enc_str) {
  int i = 0;
  struct TNode *t = tree;

  for (i = 0; enc_str[i]; i++) {
    if (enc_str[i] == '0')
      t = t -> l;
    else if (enc_str[i] == '1')
      t = t -> r;

    if (t -> c) {
      printf("%c", t -> c);
      t = tree;
    }
  }
  printf("\n");
}

// Main program: builds Huffman tree and encodes/decodes text
int main() {
  char str[1000] = {};
  struct LNode *h = NULL;
  struct TNode *tree = NULL;
  char enc_str[10000] = {};
  int i = 0, t = 0;

  printf("Enter string of text <1000 char ([Letters] [Numbers] [,] [.] [SPACE]):\n");
  scanf("%[^\n]999s", str);

  h = createList(str);

  // Build Huffman tree by repeatedly merging lowest frequency pairs
  while (h -> next != NULL) {
    h = mergeList(h);
  }

  tree = h -> tree;
  free(h);

  // Allow user to encode or decode
  printf("Choose Input:\n1. Encode\n2. Decode\n");
  scanf("%d", &i);

  switch (i) {
    case 1:
    printf("Enter string to Encode:\n");
    scanf(" %[^\n]s", str);

    for (i = 0; str[i]; i++, enc_str[0] = 0) {
      t = encode(tree, str[i], enc_str, 0);
      if (t == 0) {
        printf("\nINVALID LETTER\n");
        return 0;
      }
      printf("%s", enc_str);
    }
    printf("\n");
    break;

    case 2:
    printf("Enter string to Decode:\n");
    scanf("%s", enc_str);
    decode(tree, enc_str);
    break;
    default: printf("INVALID INPUT!!!!");
  }
}
