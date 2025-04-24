package leveldb

import (
	"log"
	"sync"

	"github.com/syndtr/goleveldb/leveldb"
	"github.com/syndtr/goleveldb/leveldb/opt"
)

type DBManager struct {
	db *leveldb.DB
	mu sync.Mutex
}

func (dbm *DBManager) Close() {
	if dbm.db != nil {
		dbm.mu.Lock()
		defer dbm.mu.Unlock()
		if err := dbm.db.Close(); err != nil {
			log.Println("Failed to close database:", err)
		}
	}
}

func (dbm *DBManager) WriteImage(batch *leveldb.Batch, wo *opt.WriteOptions) error {
	dbm.mu.Lock()
	defer dbm.mu.Unlock()
	return dbm.db.Write(batch, wo)
}
func (dbm *DBManager) GetImage(imageID string) ([]byte, error) {
	data, err := dbm.db.Get([]byte(imageID), nil)
	if err != nil {
		return nil, err
	}
	return data, nil
}
func (dbm *DBManager) RemoveImage(imageID string) error {
	return dbm.db.Delete([]byte(imageID), nil)
}

func (dbm *DBManager) WriteFile(fileID string, data []byte) error {
	dbm.mu.Lock()
	defer dbm.mu.Unlock()
	return dbm.db.Put([]byte(fileID), data, nil)
}
func (dbm *DBManager) GetFile(fileID string) ([]byte, error) {
	data, err := dbm.db.Get([]byte(fileID), nil)
	if err != nil {
		return nil, err
	}
	return data, nil
}
func (dbm *DBManager) RemoveFile(fileID string) error {
	return dbm.db.Delete([]byte(fileID), nil)
}

func NewDBManager(dbName string) *DBManager {
	db, err := leveldb.OpenFile(dbName, nil)
	if err != nil {
		log.Panic("Failed to open database: ", err)
		return nil
	}
	return &DBManager{db: db}
}
