package com.lc.test;

import com.mysql.clusterj.annotation.Column;
import com.mysql.clusterj.annotation.PersistenceCapable;
import com.mysql.clusterj.annotation.PrimaryKey;

public class TestTable {

  public static final String TABLE_NAME = "test_table";
  public static final String ATTR1 = "ATTR1";
  public static final String ATTR2 = "ATTR2";

  @PersistenceCapable(table = TABLE_NAME)
  public interface TestTableDTO {

    @PrimaryKey
    @Column(name = ATTR1)
    int getAttr1();
    void setAttr1(int key);

    @Column(name = ATTR2)
    byte[] getAttr2();
    void setAttr2(byte[] val);

  }
}
