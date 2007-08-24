#ifndef _DATABASECLEANER_H
#define _DATABASECLEANER_H

class DatabaseCleaner : public Singleton<DatabaseCleaner>
{
public:
	void Run();

protected:
	void CleanCharacters();
	void CleanWorld();
	void Optimize();
};

#endif

