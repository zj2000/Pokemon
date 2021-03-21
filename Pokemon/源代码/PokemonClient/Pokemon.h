#ifndef POKEMON_H
#define POKEMON_H


#include <vector>
#include <easyx.h>

#include "Move.h"

enum class Attribute { highHP, highPhysicalAttcak, highPhysicalDefence, highSpecialAttack, highSpecialDefence, highSpeed };

extern IMAGE imgFront[48];//С����ս��ʱ��ʾ��ͼ��ǰ����,ԭͼ
extern IMAGE imgFrontCover[48];//С����ս��ʱ��ʾ��ͼ��ǰ����,����ͼ
extern IMAGE imgBack[48];//С����ս��ʱ��ʾ��ͼ���󷽣�,ԭͼ
extern IMAGE imgBackCover[48];//С����ս��ʱ��ʾ��ͼ���󷽣�,����ͼ

class Pokemon
{
protected:
	//������أ�����֮�����������ֵ��
	int myID;	//����С������
	int level;	//�ȼ�
	int EXP;	//����ֵ

	//����ֵ
	int healthPoint;	//����ֵ	
	int attackPoint;	//����ֵ--����
	int defencePoint;	//����ֵ--����
	int specialAttack;	//�ع�ֵ--����
	int specialDefence;	//�ط�ֵ--����
	int attackSpeed;	//�ٶ�
	int attackInterval;	//�������(= �̶�ʱ�� �� �ٶ�),ʱ�䵥λ�Ǻ���

	//��ս���
	int currentHP;		//��սʱʣ�������ֵ
	int accuracyLevel;	//�����ʵȼ�����ս��ʼ�ͽ���ʱ��Ϊ0�����ݶ�ս�������仯��ȡֵ��Χ��0��6��������Χȡ��Χ����ֵ
	int evasionLevel;	//�����ʵȼ�����ս��ʼ�ͽ���ʱ��Ϊ0�����ݶ�ս�������仯��ȡֵ��Χ��0��6��������Χȡ��Χ����ֵ
	bool isAtCriticalState;//�Ƿ�������Ҫ��״̬���ǵĻ�������߱�����
	int criticalLeftTimes;//ʣ����Դ�������Ҫ��״̬�Ĵ���
	clock_t lastAttackTime;//�ϴγ��е�ʱ�䣬���ڼ�����
	std::string moveName; //ĳ�γ��е���ʽ����
	int damage;		//ĳ�γ��и��Է��������˺�
	bool isHit;		//�Ƿ���жԷ�
	bool isCritical;//�Ƿ񱩻�


	//������أ�ֵ�����и�������屦����һ�£�
	int speciesID;	//����ID
	int baseEXP;	//��������ֵ�����ڼ����սʤ����ľ���ֵ����
	int growPoints;	//����ʱ����ָ��������ֵ
	std::string name;//��������	

	Attribute mainAttribute;//�����ԣ��߹�/�߷�/��Ѫ/����
	Type PokemonType;//���ˮ/��/��/��/��

	int learnedMoves[4];//��С�������յ���ʽ,һֻ���������ֻ��ͬʱ��ס4����ʽ

public:
	Pokemon(const Pokemon& copy);
	Pokemon(int id, std::string name, std::string type, std::string mainA, int base, int grow, int attr[6],int move[4]);//����Ĺ��캯��
	Pokemon(int myID, int speciesID, int level, int EXP, int attr[6]);//����Ĺ��캯��

	//ս�����
	void UpdateAttribute();//��������
	int AutoMove() const;//�������,����ֵΪ��ʽ����ʽ���±�
	virtual int Attack(Pokemon* enemy);//��������,������Է����˺��ӳ�,�������˺�ֵ
	void Damage(int damage);//С�����ܵ�����ʱ���˺�����
	int UpdateExp(Pokemon* enemyPokemon);//��սʤ���󣬸��¾���ֵ,���Զ�����
	void Reset();//ս����ʼǰ����ս�������ֵ
	int ComputeFinalDamage(double bonus, Move* attackMove, Pokemon* enemy) const;
	double ComputeAccuracyRate() const;//���㲢����������
	double ComputeEvasionRate(Pokemon* enemy) const;//���㲢����������
	double ComputeCriticalRate(Move* attackMove);//���㲢���ر�����
	bool whetherHit(Pokemon* enemy, int moveAccuracy) const;//�жϲ����ض�սʱ�Ƿ����жԷ�
	double GetRandomFactor(int min) const;//����һ�����С���������С���ķ�Χ��min/100 - 1
	int GetCurrentHP() const;//����ʣ��Ѫ��

	//��ñ�������Ϣ
	Type GetType()const;//���
	int GetLevel() const;//�ȼ�
	int GetIndividualID()const;//����ID
	int GetSpeiesID()const;//����ID
	std::string GetName() const;//����
	int GetSpeed() const;//�ٶ�
	std::string GetTypeString(Type type) const;
	std::string GetAttributeString() const;
	std::string GetPokemonInfo() const;//�ۺ���Ϣ

	//�������
	void DrawPkmInfo() const;//�����ۺ���Ϣ��������
	void DrawPkmList(int curPage, int total, int curCheckSeq) const;//���Ʊ����α����б�
	void DrawLose(int seq) const;//��ӡҪʧȥ�ı�����
	void DrawBattle(bool isAtFront) const;//��ӡս�������Ϣ������

};

#endif // !POKEMON_H
