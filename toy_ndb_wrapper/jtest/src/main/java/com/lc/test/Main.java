package com.lc.test;

import com.mysql.clusterj.ClusterJException;
import com.mysql.clusterj.ClusterJHelper;
import com.mysql.clusterj.Session;
import com.mysql.clusterj.SessionFactory;

import java.util.Properties;

public class Main {
  public SessionFactory sessionFactory;
  Session session;

  public static void main(String argv[]) throws Exception {
    new Main().test();
  }

  public void test() throws Exception {
    setUpDBConnection();
    session = sessionFactory.getSession();

    long startTime = System.currentTimeMillis();

    int iterations = 100000;
    for (int i = 0; i < iterations; i++) {
      writeRow(i, i);
    }

    for (int i = 0; i < iterations; i++) {
      findRow( i);
    }

    long endTime = System.currentTimeMillis();
    System.out.println("Time taken: "+(endTime - startTime)+" ms");
  }

  public void writeRow(int key, int value) {
    TestTable.TestTableDTO persistable = session.newInstance(TestTable.TestTableDTO.class);
    persistable.setAttr1(key);
    persistable.setAttr2(value);
    session.savePersistent(persistable);
    session.release(persistable);
  }

  public void findRow(int key) {
    TestTable.TestTableDTO lTable = (TestTable.TestTableDTO) session.find(TestTable.TestTableDTO.class,
      key);
    if (lTable.getAttr2() < 0) {
      System.err.println("Wrong value read for index " + key);
    }
    session.release(lTable);
  }

  public void setUpDBConnection() throws Exception {
    Properties props = new Properties();
    props.setProperty("com.mysql.clusterj.connectstring", "localhost");
    props.setProperty("com.mysql.clusterj.database", "test_db");
    props.setProperty("com.mysql.clusterj.connect.retries", "4");
    props.setProperty("com.mysql.clusterj.connect.delay", "5");
    props.setProperty("com.mysql.clusterj.connect.verbose", "1");
    props.setProperty("com.mysql.clusterj.connect.timeout.before", "60");
    props.setProperty("com.mysql.clusterj.connect.timeout.after", "5");
    props.setProperty("com.mysql.clusterj.max.transactions", "1024");
    props.setProperty("com.mysql.clusterj.connection.pool.size", "1");
    props.setProperty("com.mysql.clusterj.max.cached.instances", "256");
    props.setProperty("com.mysql.clusterj.connection.reconnect.timeout", "5");

    try {
      sessionFactory = ClusterJHelper.getSessionFactory(props);
    } catch (ClusterJException ex) {
      throw ex;
    }
    System.out.println("Connected");
  }
}
