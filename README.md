< Une deque est une structure de données qui généralise les files et les piles : elle permet
l’enfilage et le défilage des deux côtés. Une deque a deux bouts, le haut et le bas, et quatre opéra-
tions : enfiler en bas, enfiler en haut, défiler en bas et défiler en haut. Il est facile d’implémenter
une deque comme une file circulaire ou comme une liste doublement chaînée. >
< L’ordonnanceur contient n threads et n deques, une par thread. Initialement,
toutes les deques sont vides sauf une, qui contient la tâche initiale. Lorsqu’un thread a fini d’exé-
cuter une tâche, il défile la tâche qui est en bas de la deque qui lui est associée ; si cette deque
est vide, il effectue une étape de work stealing (décrite ci-dessous). S’il n’a toujours pas réussi à
trouver du travail, il s’endort pendant 1 ms, puis tente une étape de work stealing de nouveau.
Un processus qui est dans la phase de work stealing se comporte de la façon suivante. Il commence par tirer au hasard un thread k, 
et essaie de défiler une tâche du haut de la deque associée
à ce thread. Si cette opération réussit (la deque n’était pas vide), il effectue la tâche ainsi obtenue
puis continue à exécuter l’algorithme normal. Sinon, il itère à travers tous les autres threads et essaie de voler du travail 
à l’un d’eux. Si toutes les deques étaient vides, le work stealing a échoué, et le thread s’endort.
Lorsqu’une nouvelle tâche est créée, elle est enfilée en bas de la deque associée au thread qui
a fait l’appel à sched_spawn (la création de tâches est locale). L’ordonnanceur termine lorsque
tous les threads sont oisifs : ils sont tous en train de dormir après avoir échoué à voler du travail. >

-   the deque used is a double chained list
-   compile with : make
-   execute with : ./run
-   options of execution :
    +  sequential : ./run -s
    +  define number of threads (if not defined by default it will take the default number of threads of the system) : ./run -t number
    +  define the size of the array that will be used : ./run -n number

#for the same example of "calculate a quicksort in parallel using multiple threads" but using 1 shared stack (LIFO) : 
https://github.com/rafikFares/Scheduler-with-one-LIFO/tree/master
