package task7;

import util.BSTOperations;
import util.BinarySearchTreeNode;

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class MyMain {
    public static void main(String[] args) throws ExecutionException, InterruptedException {
        BinarySearchTreeNode<Integer> binarySearchTree = new BinarySearchTreeNode<>(3);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 6);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 9);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 2);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 10);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 1);

        ExecutorService tpe = Executors.newFixedThreadPool(4);
        CompletableFuture<String> completableFuture = new CompletableFuture<>();
        AtomicInteger counter = new AtomicInteger(0);
        counter.incrementAndGet();
        tpe.submit(new MyRunnable(tpe, binarySearchTree, 5, counter, completableFuture));

        var result = completableFuture.get();
        if (result != null) {
            System.out.println("Found value: " + result);
        } else {
            System.out.println("Value was not found");
        }
    }
}
