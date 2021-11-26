package task7;

import util.BinarySearchTreeNode;

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
    private final ExecutorService tpe;
    private BinarySearchTreeNode<Integer> binarySearchTree;
    private Integer value;
    private final AtomicInteger counter;
    private final CompletableFuture<String> completableFuture;

    public MyRunnable(
            ExecutorService tpe,
            BinarySearchTreeNode<Integer> binarySearchTree,
            Integer value,
            AtomicInteger counter, CompletableFuture<String> completableFuture
    ) {
        this.tpe = tpe;
        this.binarySearchTree = binarySearchTree;
        this.value = value;
        this.counter = counter;
        this.completableFuture = completableFuture;
    }

    @Override
    public void run() {
        if (binarySearchTree != null) {
            if (value.equals(binarySearchTree.getValue())) {
                completableFuture.complete(binarySearchTree.getValue().toString());
                tpe.shutdown();
            } else if (binarySearchTree.getValue().compareTo(value) > 0) {
                counter.incrementAndGet();
                Runnable t = new MyRunnable(tpe, binarySearchTree.getLeft(), value, counter, completableFuture);
                tpe.submit(t);
            } else {
                counter.incrementAndGet();
                Runnable t = new MyRunnable(tpe, binarySearchTree.getRight(), value, counter, completableFuture);
                tpe.submit(t);
            }
        }

        int left = counter.decrementAndGet();
        if (left == 0) {
            completableFuture.complete(null);
            tpe.shutdown();
        }
    }
}
