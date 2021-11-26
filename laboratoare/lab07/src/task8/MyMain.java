package task8;

import util.BSTOperations;
import util.BinarySearchTreeNode;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.ForkJoinPool;

public class MyMain {
    public static void main(String[] args) throws ExecutionException, InterruptedException {
        ForkJoinPool forkJoinPool = new ForkJoinPool(4);
        BinarySearchTreeNode<Integer> binarySearchTree = new BinarySearchTreeNode<>(3);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 6);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 9);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 2);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 10);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 1);

        MyTask task = new MyTask(binarySearchTree);
        forkJoinPool.execute(task);
        System.out.println(task.get());
    }
}
