package mysqlmanager

import (
	"database/sql"
	"fmt"
	"log"

	_ "github.com/go-sql-driver/mysql"
)

type MysqlManager struct {
	Connections_ *sql.DB
}

func (mysql_ *MysqlManager) Close() {
	if mysql_.Connections_ != nil {
		mysql_.Connections_.Close()
	}
}

func (mysql_ *MysqlManager) ExecMysqlTask(sql string, args ...interface{}) *sql.Rows {
	rows, err := mysql_.Connections_.Query(sql, args...)
	if err != nil {
		fmt.Println("mysql exec fail:", err, sql)
		return nil
	}
	return rows
}

func (mysql_ *MysqlManager) GetOnlyIsSuccess(sql string, args ...interface{}) bool {
	_, err := mysql_.Connections_.Exec(sql, args...)
	if err != nil {
		fmt.Println("mysql insert fail:", err, sql)
		return false
	}
	return true
}

func NewMysqlManager(connNB int) *MysqlManager {
	m := &MysqlManager{}
	var err error
	dsn := "IMuser:zihang623@tcp(127.0.0.1:3306)/IM"
	m.Connections_, err = sql.Open("mysql", dsn)
	if err != nil {
		log.Panic("mysql error", err)
		return nil
	}
	return m
}
