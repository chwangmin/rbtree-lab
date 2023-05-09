#include "rbtree.h"

#include <stdlib.h>
#include <stdbool.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  
  // nil 노드 생성 *nil을 사용해서 nil의 값 지정, (node_t *) 사용해서 calloc으로 지정한 크기 타입 캐스팅
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));

  // nil 노드는 무조건 black
  nil->color = RBTREE_BLACK;

  // 첫 초기화 트리이기 때문에 트리의 root와 nil을 nil 노드로 지정 이것으로 nil 노드를 하나만 써도 됨.
  p->nil = p->root = nil;
  return p;
}

void delete_rbtree(rbtree *t) {
  node_t *node = t->root;
  // tree의 루트노드가 tree의 nil 노드가 아니라면 key값을 가진 node가 존재한다는 의미로 루트 노드 포함해서 아래 노드 모두 삭제를 위한 함수 실행
  if (node != t->nil){
    // tree와 tree의 루트 노트를 입력
    tree_delete_traverse(t,node);
  }
  // 트리의 nil 노드를 할당 해제 해준다.
  free(t->nil);
  // 트리를 할당 해제 한다.
  free(t);
}

void tree_delete_traverse(rbtree *t, node_t *node){
  // 만약 노드의 왼쪽 노드가 nil 노드가 아니라면 그 왼쪽 노드로 재귀(처음이면 루트노드)
  if (node->left != t->nil){
    tree_delete_traverse(t,node->left);
  }
  // 만약 노드의 오른쪽 노드가 nil 노드가 아니라면 그 오른쪽 노드로 재귀
  if (node->right != t->nil){
    tree_delete_traverse(t,node->right);
  }
  // 노드를 할당 해제 해준다.
  free(node);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // node 동적 할당 calloc으로
  node_t *new_node = (node_t *)calloc(1,sizeof(node_t));
  // new_node 는 처음에 RED로 무조건 설정
  new_node->color = RBTREE_RED;
  // 왼쪽과 오른쪽은 nil 노드로 설정
  new_node->left = new_node->right = t->nil;
  // key 값(현재의 숫자)으로 설정
  new_node->key = key;
  
  // 현재노드를 루트 노드로 설정
  node_t *current_node = t->root;
  
  // 만약 현재 노드가 nil 노드를 안가리킬 때 까지
  while (current_node != t->nil){
    // key값이 현재노드의 값보다 작으면
    if (key < current_node->key){
      // 현재 노드의 왼쪽 노드가 nil 노드를 가리킨다면
      if (current_node->left == t->nil){
        // 현재 노드의 왼쪽을 new_node로 지정 후 끝내기
        current_node->left = new_node;
        break;
      }
      // 현재 노드를 현재 노드의 왼쪽으로 (왼쪽 탐색)
      current_node = current_node->left;
    }
    // key 값이 현재노드의 값보다 크거나 같다면
    else{
      // 현재 노드의 오른쪽이 nil노드를 가리킨다면
      if (current_node->right == t->nil){
        // 현재노드의 오른쪽에 new_node를 지정 후 멈추기
        current_node->right = new_node;
        break;
      }
      // 현재 노드를 현재노드의 오른쪽으로 지정 (오른쪽 탐색)
      current_node = current_node->right;
    }
  }
  //새로운 노드의 부모를 현재 노드로 설정.
  new_node->parent = current_node;
  // 만약 현재노드의 값이 nil 노드라면 (루트노드가 nil노드라면 트리에 nil노드 외에 아무 노드도 없다는 뜻이기에)
  if (current_node == t->nil){
    // 트리의 루트 노드를 new_node로 지정
    t->root = new_node;
  }
  // 삽입 case 1,2,3 확인
  rbtree_insert_fixup(t,new_node);
  return new_node;
}

void rbtree_insert_fixup(rbtree *t, node_t *node){
  // 만약 노드가 루트 노드라면 컬러를 규칙 #2번에 따라 루트 노드의 색을 블랙으로 변경
  if (node == t->root){
    node->color = RBTREE_BLACK;
    return;
  }
  node_t *parent_node = node->parent;
  bool is_left_node = node == parent_node->left; 

  // 만약 부모의 노드가 검은색이면 속성 위반이 없기에 그냥 끝.
  if (parent_node->color == RBTREE_BLACK){
    return;
  }

  node_t *grand_parent_node = parent_node->parent; 
  bool is_left_parent_node = parent_node == grand_parent_node->left;

  node_t *uncle_node;

  if (is_left_parent_node){
    uncle_node = grand_parent_node->right;
  }else{
    uncle_node = grand_parent_node->left;
  }

  // case 1 실행 (만약 삼촌이 빨간색 노드라면)
  if (uncle_node->color == RBTREE_RED){
    parent_node->color = RBTREE_BLACK;
    uncle_node->color = RBTREE_BLACK;
    grand_parent_node->color = RBTREE_RED;
    rbtree_insert_fixup(t,grand_parent_node);
    return;
  }

  // 부모의 노드가 조부모의 왼쪽일때
  if(is_left_parent_node){
    if(is_left_node){ // 부모가 왼쪽 자식이고 현재 노드가 왼쪽자식일때 -> case 3
      rotate_R(t,parent_node); // rotated 공부하기
      parent_node->color = RBTREE_BLACK;
      parent_node->right->color = RBTREE_RED;
      return;
    }else{ // 부모가 왼쪽 자식이고 현재 노드가 오른쪽자식일때 -> case 2
      rotate_L(t,node);
      rotate_R(t,node);
      node->color = RBTREE_BLACK;
      node->right->color = RBTREE_RED;
      return;
    }
  }
  // 부모의 노드가 조부모의 오른쪽일때
  else{
    if(is_left_node){ // 부모가 오른쪽 자식이고 현재 노드가 왼쪽자식일때 -> case 2
      rotate_R(t,node);
      rotate_L(t,node);
      node->color = RBTREE_BLACK;
      node->left->color = RBTREE_RED;
      return;
    }else{ // 부모가 오른쪽 자식이고 현재 노드가 오른쪽자식일떄 -> case 3
      rotate_L(t,parent_node);
      parent_node->color = RBTREE_BLACK;
      parent_node->left->color = RBTREE_RED;
      return;
    }
  }
}

void rotate_L(rbtree *t,node_t *node){
  node_t *parent_node = node->parent;
  node_t *grand_parent_node = parent_node->parent;
  node_t *left_node = node->left;

  // 부모 노드가 루트 노드였다면
  if (parent_node == t->root){
    // 루트 노드를 노드로 한다.
    t->root = node;
  } else {
    // 조부모의 노드의 왼쪽에 부모 노드라면 그 자리를 노드로 넣기 위해 if 문 사용
    if (grand_parent_node->left == parent_node){
      grand_parent_node->left = node;
    } else {
      grand_parent_node->right = node;
    }
  }
  // 노드의 부모를 조부모로 바로 연결 (G <-> N 양방향 연결)
  node->parent = grand_parent_node;
  // 부모 노드를 노드의 왼쪽 자식으로 설정 하기 (P <-> N 양방향 연결)
  parent_node->parent = node;
  node->left = parent_node;                                        
  // 노드의 원래의 왼쪽 자식은 부모의 오른쪽 자식으로 설정해야함. 
  parent_node->right = left_node;
  left_node->parent = parent_node;                                                                                                                                                                                                                                                                                                           
}

void rotate_R(rbtree *t,node_t *node){
  node_t *parent_node = node->parent;
  node_t *grand_parent_node = parent_node->parent;
  node_t *right_node = node->right;

  // 부모 노드가 루트 노드였다면
  if (parent_node == t->root){
    // 루트 노드를 노드로 한다.
    t->root = node;
  } else {
    // 조부모의 노드의 왼쪽에 부모 노드라면 그 자리를 노드로 넣기 위해 if 문 사용
    if (grand_parent_node->left == parent_node){
      grand_parent_node->left = node;
    } else {
      grand_parent_node->right = node;
    }
  }
  // 노드의 부모를 조부모로 바로 연결 (G <-> N 양방향 연결)
  node->parent = grand_parent_node;
  // 부모 노드를 노드의 오른쪽 자식으로 설정 하기 (P <-> N 양방향 연결)
  parent_node->parent = node;
  node->right = parent_node;
  right_node->parent = parent_node;                                     
  // 노드의 원래의 오른쪽 자식은 부모의 왼쪽 자식으로 설정해야함. 
  parent_node->left = right_node;    
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *current_node = t->root;
  while (current_node != t->nil){
    if (current_node->key == key){
      return current_node;
    } else if (key < current_node->key){
      current_node = current_node->left;
    } else {
      current_node = current_node->right;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // 현재 노드를 루트 노드로 설정
  node_t *current_node = t->root;
  // 왼쪽 끝까지 노드를 확인
  while (current_node->left != t->nil){
    current_node = current_node->left;
  }
  return current_node;
}

node_t *rbtree_max(const rbtree *t) {
  // 현재 노드를 루트 노드로 설정
  node_t *current_node = t->root;
  // 오른쪽 끝까지 노드를 확인
  while (current_node->right != t->nil){
    current_node = current_node->right;
  }
  return current_node;
}

int rbtree_erase(rbtree *t, node_t *check_node) {
  node_t *successor_node; // 삭제할 노드의 대신 들어갈 노드 (이 노드는 삭제될 것임.)
  node_t *replace_node; // 삭제할 노드에 후보자 노드가 들어가면 원래의 후보자 노드의 오른쪽 트리들을 재설정해줘야함.
  node_t *parent_successor_node;

  // Step 1) check_node 삭제 후 대체할 successor 노드 찾기
  // 만약 삭제할 노드의 자식이 2개 이상일때 (왼쪽과 오른쪽 노드가 닐 노드가 아닐때)
  if (check_node->left != t->nil && check_node->right != t->nil){
    // 후보 노드는 삭제할 노드의 오른쪽 트리의 가장 작은 값 (오른쪽 노드가 확실히 있기 때문에 확인)
    successor_node = rbtree_successor_find(t,check_node->right);
    replace_node = successor_node->right;
    check_node->key = successor_node->key;
  }else{ // 자식이 없거나 하나일 경우
    successor_node = check_node;
    if (check_node->right != t->nil){
      replace_node = successor_node->right;
    } else{
      replace_node = successor_node->left;
    }
  }
  // 후보자의 부모의 왼쪽에 replace 노드를 양방향 연결 해야 하기 때문에 지정
  parent_successor_node = successor_node->parent;

  // Step 2) seccessor 노드 제거하기
  if (successor_node == t->root){
    t->root = replace_node;
    t->root->color = RBTREE_BLACK;
    free(successor_node);
    return 0;
  }

  // Step 2-1) seccessor 부모와 seccessor 자식 이어주기
  bool is_successor_black = successor_node->color;
  bool is_successor_left = parent_successor_node->left == successor_node;

  // Step 2-1-1) 자식연결
  if (is_successor_left){
    parent_successor_node->left = replace_node;
  } else{
    parent_successor_node->right = replace_node;
  }

  // Step 2-1-2) 부모도 연결
  replace_node->parent = parent_successor_node;
  free(successor_node);

  // Step 3) 불균형 복구 함수 호출
  if (is_successor_black){
    rbtree_erase_fixup(t,parent_successor_node,is_successor_left);
  }

  return 0;
}

void rbtree_erase_fixup(rbtree *t, node_t *parent_node, bool is_node_left){
  // 검은색이 추가된 부분의 노드를 찾음
  node_t *extra_black = is_node_left ? parent_node->left:parent_node->right;

  // 검은색이 추가된 노드의 색이 빨간색이면 그냥 검은색으로 바꿔주고 끝냄.
  if (extra_black->color == RBTREE_RED){
    extra_black->color = RBTREE_BLACK;
    return;
  }

  // 형제의 노드는 부모의 자식인데 내가 아닌 자식이기에 반대로 설정
  node_t *sibling_node = is_node_left ? parent_node->right:parent_node->left;
  // 형재의 왼쪽 노드와 오른쪽 노드를 각각 설정
  node_t *sibling_left_node = sibling_node->left;
  node_t *sibling_right_node = sibling_node->right;

  // Case 1) 형재의 색이 RED라면
  if (sibling_node->color == RBTREE_RED){
    if (is_node_left){
      rotate_L(t,sibling_node);
    }else{
      rotate_R(t,sibling_node);
    }
    exchange_color(sibling_node,parent_node);
    rbtree_erase_fixup(t,parent_node,is_node_left);
    return;
  }

  //exra 블랙에서 가장 가까운 노드
  node_t *near = is_node_left? sibling_left_node : sibling_right_node;
  node_t *distant = is_node_left? sibling_right_node : sibling_left_node;

  // Case 3) 노드가 왼쪽 일때
  if (is_node_left && near->color==RBTREE_RED && distant->color==RBTREE_BLACK){
    rotate_R(t,near);
    exchange_color(sibling_node,near);
    rbtree_erase_fixup(t,parent_node,is_node_left);
    return;
  }

  // Case 4) 노드가 왼쪽 일 때
  if (is_node_left && distant->color==RBTREE_RED){
    rotate_L(t,sibling_node);
    exchange_color(sibling_node,parent_node);
    distant->color = RBTREE_BLACK;
    return;
  }

  // Case 3) // 노드가 오른쪽일 때
  if (near->color==RBTREE_RED && distant->color==RBTREE_BLACK){
    rotate_L(t,near);
    exchange_color(sibling_node, near);
    rbtree_erase_fixup(t,parent_node,is_node_left);
    return;
  }

  // Case 4) // 노드가 오른쪽일 때
  if (distant->color==RBTREE_RED){
    rotate_R(t,sibling_node);
    exchange_color(sibling_node,parent_node);
    distant->color=RBTREE_BLACK;
    return;
  }

  // Case 2)
  sibling_node->color = RBTREE_RED;

  bool is_parent_left = parent_node->parent->left == parent_node;

  if (parent_node != t->root){
    rbtree_erase_fixup(t,parent_node->parent, is_parent_left);
  }
}

void exchange_color(node_t *node1, node_t *node2){
  int tmp_color = node1->color;
  node1->color = node2->color;
  node2->color = (tmp_color == RBTREE_BLACK) ? RBTREE_BLACK:RBTREE_RED;
}

node_t *rbtree_successor_find(const rbtree *t, node_t *node) {
  while (node->left != t->nil){
    node = node->left;
  }
  return node;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_t *node = t->root;
  int idx=0;
  rbtree_inOrder(t,arr,node, &idx);

  return 0;
}

void rbtree_inOrder(const rbtree *t, key_t *arr, node_t *node, int *idx){
  if(node->left != t->nil){
    rbtree_inOrder(t,arr,node->left,idx);
  }
    arr[*idx] = node->key;
    (*idx)++;
  if(node->right !=t->nil){
    rbtree_inOrder(t,arr,node->right,idx);
  }
}
