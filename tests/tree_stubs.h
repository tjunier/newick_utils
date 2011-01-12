/* functions that return tree structures independently of the parser. */

/* These are useful in two cases: i) when testing something "downstream" of the
 * parser, that assumes a correct tree is available; ii) when it is unwieldy to
 * use the parser, e.g. in automatic tests (since the parser can only read
 * FILE*s) */

struct rooted_tree;

/* ((A,B),C); */
struct rooted_tree tree_1();

/* ((A,B)f,(C,(D,E)g)h)i; */
struct rooted_tree tree_2();

/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
struct rooted_tree tree_3();

/* ((A:1,B:1.0)f:2.0,(C:1,(C:1,C:1)g:2)h:3)i;  - one clade made of three 'C's */
struct rooted_tree tree_4();

/* A tree whose root has 3 children: (A:3,B:3,(C:2,(D:1,E:1)f:1)g:1)h; */
struct rooted_tree tree_5();

/* A tree with an inner node that has three children:
 *  ((A:1,B:1,C:1)e:1,D:2)f */
struct rooted_tree tree_6();

/* A tree with some empty labels */
/* ((A:1,:1.0)f:2.0,(C:1,(D:1,E:1):2)h:3)i; */
struct rooted_tree tree_7();

/* A tree with labels longer than one char */
/* ((HRV_A1:1,HRV_A2:1.0)HRV_A:2.0,(HRV_C:1,(HRV_B1:1,HRV_B2:1)HRV_B:2):3)HRV; */
struct rooted_tree tree_8();

/* A tree with several redundant labels */
/* (((D,D)e,D)f,((C,B)g,(B,A)h)i)j; */
struct rooted_tree tree_9();

/* A completely linear tree */
/* (((((Homo_sapiens)Homo)Hominini)Homininae)Hominidae)Hominoidea; */
struct rooted_tree tree_10();

/* A partially linear tree */
/* ((((Gorilla,(Pan,Homo)Hominini)Homininae)Hominidae)Hominoidea); */
struct rooted_tree tree_11();

/* A tree some edges of which have length, and others don't */
/* ((A,B:1),C); */
struct rooted_tree tree_12();

/* A tree with short labels, out of alphabetical order */
/* ((Bee,Ant),(Eel,(Dog,Cat))); */
struct rooted_tree tree_13();

/* A top-light ladder tree */
/* (Petromyzon,(Carcharodon,(Xenopus,(Columba,(Equus,Homo)Mammalia)Amniota)Tetrapoda)Gnathostomata)Vertebrata;
 * */
struct rooted_tree tree_14();

/* A top-heavy ladder tree, isomorphic to #14 */
/* (((((Equus,Homo)Mammalia,Columba)Amniota,Xenopus)Tetrapoda,Carcharodon)Gnathostomata,Petromyzon)Vertebrata; */
struct rooted_tree tree_15();

/* A tree with labels that have spaces, or quotes */
/* (('Falco peregrinus', Falco_eleonorae),'Falco_rusticolus'); */
struct rooted_tree tree_16();
