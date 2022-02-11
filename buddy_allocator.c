#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

//NB. La radice dell'albero ha indice 0!
//restituisce il livello dell'idx
int levelIdx(size_t idx){
  return (int)floor(log2(idx+1)); //floor arrotonda al numero più vicino
}

int buddyIdx(int idx){ //mi restituisce l'indice del fratello
  if (idx == 0)
    return 0; //0 non ha fatelli
  else if (idx & 0x1)
    return idx + 1; //il fratello di 1 è 2 e viceversa
  return idx - 1;
}

int parentIdx(int idx){
  return (idx-1)/2; //il padre di 1 è 0 di 3 è 1 e così via
}

int firstIdx(int lvl){
  return (1 << lvl) - 1; //il primo indice del livello 0 è 0, del livello 1 è 1, del livello 2 è 3 e cosi via
} // return (2^lvl)-1;

//calcolo la posizione di idx nel suo livello
int startIdx(int idx){
  return (idx-(firstIdx(levelIdx(idx)))); // se idx=36 -> livello:5 e first(5)=31 -> offset: 36-31=5
}


int BuddyAllocator_init(BuddyAllocator* buddy,
                         int buddy_levels,
                         char* memory, // buffer per l'allocator
                         int memory_size,
                         char* bitmap_buf, // buffer per la bitmap
                         int bitmap_buf_size,
                         int min_bucket_size) {

  // scelta implementativa: i miei bucket sono di dimesione 8
  if (min_bucket_size < 8) {
    printf("[BuddyAllocator_init] ERRORE: min_bucket_size deve essere >= 8\n");
    return 0;
  } else if (memory_size > min_bucket_size << buddy_levels){
	  printf("\n[WARNING] Utilizzabili solo %dByte di %dByte forniti\n", min_bucket_size << buddy_levels, memory_size);
    memory_size = min_bucket_size << buddy_levels; //min_bucket_size*2^(buddy_levels) per avere un albero completo
    
  }
  
  //ho imposto che devo avere massimo 16 livelli
  if (buddy_levels>MAX_LEVELS) {
    printf("[BuddyAllocator_init] ERRORE: Numero di livelli maggiore del massimo possibile!\n");
    return 0;
  }
  
  //generazione numero di bit per allocare la bit_map, ogni bit della bitmap è un buddy
  //ex calc_size
  int num_bits = (1 << (buddy_levels + 1)) - 1 ; // numero di nodi dell'albero della bitmap: (2^(lvl+1))-1 formula da fi2
  if (bitmap_buf_size<=BitMap_getBytes(num_bits)) {
    printf("[BuddyAllocator_init] ERRORE: Memoria per la bitmap non sufficente!\n");
    return 0;
  }

  // we need room also for level 0
  buddy->buddy_levels=buddy_levels;
  buddy->memory = memory;
  buddy->memory_size = memory_size;
  buddy->min_bucket_size=min_bucket_size;


  printf("[BuddyAllocator_init]\n");
  printf("\tMEMORY_SIZE: %d bytes\n", memory_size); // (1<<buddy_levels)*min_bucket_size  (2^5)*8 = 256 bytes
  printf("\tBUDDY_LEVELS: %d\n", buddy_levels);
  printf("\tMIN_BUCKET_SIZE: %d\n", min_bucket_size);
  printf("\tBUFFER_SIZE: %d Bytes\n", BitMap_getBytes(num_bits));

  BitMap_init(&buddy->bitmap, num_bits, bitmap_buf);
  Bitmap_print(&buddy->bitmap);
  return 1;
};


//metodo per allocare memoria di dimensione size
void* BuddyAllocator_malloc(BuddyAllocator* buddy, int size) {
  size += sizeof(int); //l'indice della bitmap ha bisogno di sizeof(int) byte
  
  //controllo che il buddy_allocator abbia abbastanza memoria a disposizione
  if (buddy->memory_size < size) {
    printf("\n[BuddyAllocator_malloc] ERRORE: memoria non disponibile! hai richiesto %d Byte", size);
    return NULL;
  }
  
  //-------------------------METODO PER TROVARE UN BUDDY VUOTO------------------------------------------
  
  printf("\n[BuddyAllocator_malloc] Cerco blocco per %d Byte ...", size);
  // cerco il livello in cui inserire
  // parto dal basso
  int level = buddy->buddy_levels;
  int dim_bucket = buddy->min_bucket_size;
  for (int i = 0; i < buddy->buddy_levels; i++){
    if (dim_bucket >= size)
      break; //ho finito la ricerca, ho trovato il blocco
    else{ //non ho trovato buckets abbastanza grandi in questo livello
      dim_bucket *= 2; //il livello precedente ha buckets di dimensione doppia
      level--; //salgo di livello
    }
  }

  //scandisco da firstidx del livello per trovare un blocco libero
  int empty_buddy_idx = -1; //parto da -1 perchè 0 è l'indice della radice
  for (int i= firstIdx(level); i < firstIdx(level + 1); i++){
    if (!BitMap_bit(&buddy->bitmap, i)){ //se non è occupato
      empty_buddy_idx = i;
      printf("\n[BuddyAllocator_malloc] Disponibile blocco di %d Byte al livello %d", dim_bucket, level);
      break;
    }
  }
  //se non lo trovo ritorno NULL
  if (empty_buddy_idx < 0){
    printf("\n[BuddyAllocator_malloc] ERRORE: Impossibile allocare il blocco... memoria terminata!");
    return NULL;
  }
  
  //-----------------------------FINO A QUI---------------------------------------

//se fin'ora è andato tutto bene, allora ho allocato in un bucket ed ora devo settare i padri e i figli come occupati!
  set_parent(&buddy->bitmap, empty_buddy_idx, 1);
  set_children(&buddy->bitmap, empty_buddy_idx, 1);
  //devo generare l'indirizzo da restituire, salvo l'indice così poi che potrò farne la free facilmente
  char *p = buddy->memory + startIdx(empty_buddy_idx) * dim_bucket;
  ((int *)p)[0] = empty_buddy_idx;
  printf("\n[BuddyAllocator_malloc] Ho scelto il buddy %d che punta alla memoria %p", empty_buddy_idx, p + sizeof(int));
  Bitmap_print(&buddy->bitmap);
  return (void *)(p + sizeof(int));
}


//releases allocated memory
void BuddyAllocator_free(BuddyAllocator *buddy, void *mem){
  
  if (!mem) {
    //printf("[BuddyAllocator_free] ERRORE: Indirizzo non valido!");
    return;
  }
  printf("\n[BuddyAllocator_free] Indirizzo -> %p", mem);
  
  // recuperiamo l'indice del buddy da liberare
	int idx_to_free = *(int*)((char*)mem - sizeof(int));
	printf("\n[BuddyAllocator_free] Buddy da liberare -> %d", idx_to_free);

  //evito di liberare un buddy gia libero
  assert("\n[BuddyAllocator_free] ERRORE: Questo buddy è già libero!" && BitMap_bit(&buddy->bitmap, idx_to_free)); //die if 0 (is false)

  //prima imposto i figli a 0...
  set_children(&buddy->bitmap, idx_to_free, 0);
  //...poi rilascio il buddy e i genitori in caso il fratello sia libero
  set_parent(&buddy->bitmap, idx_to_free,0);

  Bitmap_print(&buddy->bitmap);
}

//imposta ricorsivamente il bit dei genitori nella bitmap
void set_parent(BitMap *bit_map, int node_index, int status){
  //printf(" -> Imposto il buddy %d a %d", node_index, status);
  BitMap_setBit(bit_map, node_index, status);
  if (node_index != 0){
	  if(status == 1){
		  set_parent(bit_map, parentIdx(node_index), status);
	  } else if(status == 0){
		  int bro = buddyIdx(node_index);
		  if (!BitMap_bit(bit_map, bro)){
			//printf(" -> Unisco i buddy %d e %d", node_index, bro);
			int parent_idx = parentIdx(node_index);
			set_parent(bit_map, parent_idx, status);
          }
	  }
	}
}

//imposta ricorsivamente il bit dei figli nella bitmap
void set_children(BitMap *bit_map, int node_index, int status){
  if (node_index < bit_map->num_bits){
	  //printf(" -> Imposto il buddy %d a %d", node_index, status);
    BitMap_setBit(bit_map, node_index, status);
    set_children(bit_map, node_index * 2 + 1, status);
    set_children(bit_map, node_index * 2 + 2, status);
  }
}