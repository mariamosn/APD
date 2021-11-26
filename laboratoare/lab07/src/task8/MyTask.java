package task8;

import util.BinarySearchTreeNode;

import java.util.concurrent.RecursiveTask;

public class MyTask extends RecursiveTask<Integer> {
    private BinarySearchTreeNode<Integer> binarySearchTree;

    public MyTask (BinarySearchTreeNode<Integer> binarySearchTree) {
        this.binarySearchTree = binarySearchTree;
    }

    @Override
    protected Integer compute() {
        if (binarySearchTree == null) {
            return 0;
        }

        MyTask first = new MyTask(binarySearchTree.getRight());
        MyTask second = new MyTask(binarySearchTree.getLeft());

        first.fork();
        second.fork();

        return 1 + Math.max(
                first.join(), second.join()
        );
    }
}
