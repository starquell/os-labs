import java.util.Objects;

public class Process {
  public final int id;
  public final int cpuTime;
  public final int ioBlocking;
  public int userId;

  public int cpuDone = 0;
  public int numBlocked = 0;
  public int ioNext = 0;

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;
    Process process = (Process) o;
    return id == process.id &&
            cpuTime == process.cpuTime &&
            ioBlocking == process.ioBlocking &&
            userId == process.userId &&
            cpuDone == process.cpuDone &&
            numBlocked == process.numBlocked &&
            ioNext == process.ioNext;
  }

  public Process(int id, int cpuTime, int ioBlocking, int userId) {
    this.id = id;
    this.cpuTime = cpuTime;
    this.ioBlocking = ioBlocking;
    this.userId = userId;

  }
}
