// run() is called from Scheduling.main() and is where
// the scheduling algorithm written by the user resides.
// User modification should occur within the run() function.

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.lang.ref.Reference;
import java.lang.reflect.Array;
import java.util.*;
import java.util.stream.Collectors;

public class SchedulingAlgorithm {

//  private static class NextProcessSelector {
//
//    private Map<Integer, LinkedList<Process>> process_map = new HashMap<>();
//    private List<User> users;
//    private Vector<Process> processes;
//    private List<Double> ratios = new ArrayList<>();
//    private List<Double> current_ratios;
//    private int last_user = -1;
//
//    public NextProcessSelector (final Vector<Process> input, final Vector<User> users) {
//        this.users = new LinkedList<>(users);
//        this.processes = new Vector<>(input);
//        for (var user : users) {
//          process_map.put(user.id, new LinkedList<>());
//        }
//        for (var proc : input) {
//          process_map.get(proc.userId).addLast(proc);
//        }
//        computeRatios();
//    }
//
//    private void computeRatios() {
//        ratios = users.stream().map(user -> user.process_time_part)
//                               .collect(Collectors.toCollection(ArrayList::new));
//
//        final var sum = processes.stream().mapToInt(proc -> proc.cpuTime).sum();
//        final double min = ratios.stream().mapToDouble(x -> x).min().getAsDouble();
//        final double modifier = 1 / min;
//        ratios.replaceAll(x -> x * modifier);
//        current_ratios = new ArrayList<>(ratios);
//        last_user = -1;
//    }
//
//    public Process nextProc (Process last_finished_process) {
//        boolean needRecompute = false;
//        processes.remove(last_finished_process);
//
//        for (Map.Entry<Integer, LinkedList<Process>> entry: process_map.entrySet()) {
//            entry.getValue().remove(last_finished_process);
//            if (entry.getValue().isEmpty()) {
//                users.removeIf(user -> user.id == entry.getKey());
//                needRecompute = true;
//            }
//        }
//        process_map.entrySet().removeIf(list -> list.getValue().isEmpty());
//        if (needRecompute || current_ratios.stream().allMatch(x -> Math.abs(x) < 0.001)) {
//            computeRatios();
//        }
//        last_user++;
//        while (Math.abs(current_ratios.get(last_user)) < 0.001) {
//          last_user++;
//        }
//        current_ratios.set(last_user, current_ratios.get(last_user) - 1.);
//        Process next = process_map.get(last_user).removeFirst();
//        process_map.get(last_user).addLast(next);
//        return next;
//    }
//  }
        private static int selectNextUser(ArrayList<ArrayList<Process>> proc_map, int curr_user) {
            do {
                if (curr_user + 1 < proc_map.size()) {
                    curr_user++;
                } else {
                    curr_user = 0;
                }
             }
             while (proc_map.get(curr_user).isEmpty());
             return curr_user;
        }

        private static int selectNextProcess (ArrayList<ArrayList<Process>> proc_map,
                                              ArrayList<Process> lastProcessForUser,
                                              int curr_user,
                                              Process curr_proc) {
            boolean selected = false;
            while (true) {
                for (int i = proc_map.get(curr_user).size() - 1; i >= 0; i--) {
                    curr_proc = proc_map.get(curr_user).get(i);
                    if (curr_proc.cpuDone < curr_proc.cpuTime) {
                        lastProcessForUser.set(curr_user, curr_proc);
                        selected = true;
                    }
                }
                if (!selected) {
                    curr_user = selectNextUser(proc_map, curr_user);
                } else {
                    break;
                }
            }
            return curr_user;
        }


    public static Results run(final int runTime,
                              final Vector<Process> input,
                              final Vector<User> users,
                              final int time_quantum) {
        var result = new Results();
        int comptime = 0;
        int completed = 0;
        int currentUser = 0;
        int inQuantumElapsed = 0;

        int size = input.size();
        String resultsFile = "Summary-Processes";

        result.schedulingType = "Preemptive";
        result.schedulingName = "Fair-Share";

        var processMap = new ArrayList<ArrayList<Process>>(users.size());
        var lastProcessForUser = new ArrayList<Process>(users.size());
        for (var user : users) {
            processMap.add(user.id, new ArrayList<Process>());
        }
        for (var proc : input) {
            processMap.get(proc.userId).add(proc);
        }
        for (int i = 0; i < processMap.size(); i++) {
            lastProcessForUser.add(i, processMap.get(i).get(0));
        }

        try {
            PrintStream out = new PrintStream(new FileOutputStream(resultsFile));
            Process process = lastProcessForUser.get(currentUser);
            out.println("Process: " + lastProcessForUser.get(currentUser).id + " registered... (" + process.cpuTime + " " + process.ioBlocking + " " + process.cpuDone + ")");

            while (comptime < runTime) {

                if (process.cpuDone >= process.cpuTime) {
                    completed++;
                    out.println("Process: " + lastProcessForUser.get(currentUser).id + " completed... (" + process.cpuTime + " " + process.ioBlocking + " " + process.cpuDone + ")");
                    if (completed == size) {
                        result.totalTime = comptime;
                        out.close();
                        return result;
                    }
                    int finalCurrentUser = currentUser;
                    processMap.get(currentUser).removeIf(proc -> proc.equals(lastProcessForUser.get(finalCurrentUser)));
                    var prevUser = currentUser;
                    currentUser = selectNextProcess(processMap, lastProcessForUser, currentUser, process);
                    process = lastProcessForUser.get(currentUser);
                    if (prevUser != currentUser) {
                        inQuantumElapsed = 0;
                    }
                    out.println("Process: " + lastProcessForUser.get(currentUser).id + " registered... (" + process.cpuTime + " " + process.ioBlocking + " " + process.cpuDone + ")");
                }
                if (process.ioBlocking == process.ioNext) {
                    out.println("Process: " + lastProcessForUser.get(currentUser).id + " I/O blocked... (" + process.cpuTime + " " + process.ioBlocking + " " + process.cpuDone + ")");
                    process.numBlocked++;
                    process.ioNext = 0;
                 //   previousProcess = lastProcessForUser.get(currentUser);
                    var prevUser = currentUser;
                    currentUser = selectNextProcess(processMap, lastProcessForUser, currentUser, process);
                    process = lastProcessForUser.get(currentUser);
                    if (prevUser != currentUser) {
                        inQuantumElapsed = 0;
                    }
                    out.println("Process: " + lastProcessForUser.get(currentUser).id + " registered... (" + process.cpuTime + " " + process.ioBlocking + " " + process.cpuDone + ")");
                }
                process.cpuDone++;
                if (process.ioBlocking > 0) {
                    process.ioNext++;
                }
                comptime++;
                inQuantumElapsed++;
                if (inQuantumElapsed == time_quantum) {
                    currentUser = selectNextUser(processMap, currentUser);
                    inQuantumElapsed = 0;
                    process = lastProcessForUser.get(currentUser);
                    out.println("Process: " + lastProcessForUser.get(currentUser).id + " registered... (" + process.cpuTime + " " + process.ioBlocking + " " + process.cpuDone + ")");
                }
            }
            out.close();
        } catch (IOException e) { /* Handle exceptions */ }

        result.totalTime = comptime;
        return result;
    }
}
