#pragma once

#include <FishEngine/Serialization/Archive.hpp>
#include <FishEngine/FishEngine2.hpp>

#include <set>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <regex>

#include <yaml-cpp/yaml.h>

using namespace FishEngine;

namespace FishEditor
{
	class YAMLInputArchive : public InputArchive
	{
	public:
		YAMLInputArchive() = default;

		std::vector<Object*> LoadAllFromString(const std::string& str);


		Object* GetObjectByFileID(int64_t fileID)
		{
			auto it = m_FileIDToObject.find(fileID);
			if (it == m_FileIDToObject.end())
			{
				// fileID not found
				abort();
			}
			return it->second;
//			return m_FileIDToObject[fileID];
		}

		const std::map<int64_t, Object*>& GetFileIDToObject() const
		{
			return m_FileIDToObject;
		};

	protected:

//		bool Skip() override
//		{
//			return !CurrentNode();
//		}

		virtual Object* DeserializeObject() override;
		
		virtual void Deserialize(short & t) override			{ t = CurrentNode().as<short>(); }
		virtual void Deserialize(unsigned short & t) override	{ t = CurrentNode().as<unsigned short>(); }
		virtual void Deserialize(int & t) override				{ t = CurrentNode().as<int>(); }
		virtual void Deserialize(unsigned int & t) override		{ t = CurrentNode().as<unsigned int>(); }
		virtual void Deserialize(long & t) override				{ t = CurrentNode().as<long>();}
		virtual void Deserialize(unsigned long & t) override	{ t = CurrentNode().as<unsigned long>();}
		virtual void Deserialize(long long & t) override		{ t = CurrentNode().as<long long>();}
		virtual void Deserialize(unsigned long long & t) override { t = CurrentNode().as<unsigned long long>();}
		virtual void Deserialize(float & t) override			{ t = CurrentNode().as<float>();}
		virtual void Deserialize(double & t) override			{ t = CurrentNode().as<double>();}
		virtual void Deserialize(bool & t) override				{ t = (CurrentNode().as<int>() == 1);}
		virtual void Deserialize(std::string & t) override	{
			try {
				t = CurrentNode().as<std::string>();
			}
			catch (const std::exception& e) {
				t = "";
			}
		}

//		virtual void Get(std::string& t) override
//		{
//			t = CurrentNode().as<std::string>();
//		}
//		virtual void Get(int& t) override
//		{
//			t = CurrentNode().as<int>();
//		}
//		virtual void Get(float& t) override
//		{
//			t = CurrentNode().as<float>();
//		}
//		virtual void Get(bool& t) override
//		{
//			t = CurrentNode().as<int>() == 1;
//		}

		// Map
		virtual bool MapKey(const char* name) override
		{
			auto current = CurrentNode();
			assert(current.IsMap());
			auto node = current[name];
			if (!node)
			{
				LogWarning(Format("Key [{}] not found!", name));
			}
			PushNode(node);

			return !(!node);
		}

		virtual void AfterValue() override
		{
			PopNode();
		}
		
		// Map
		virtual int BeginMap() override
		{
			auto current = CurrentNode();
			assert(current.IsMap());
			int size = std::distance(current.begin(), current.end());
			m_mapIterator = current.begin();
			return size;
		}
		
		

		// Sequence
		virtual int BeginSequence() override
		{
			auto current = CurrentNode();
			assert(current.IsSequence());
			int size = std::distance(current.begin(), current.end());
			m_sequenceIterator = current.begin();
			return size;
		}
		virtual void BeginSequenceItem() override
		{
			PushNode(*m_sequenceIterator);
		}

		virtual void AfterSequenceItem() override
		{
			PopNode();
			m_sequenceIterator++;
		}
		virtual void EndSequence() override
		{
//			PopNode();
		}

		void PushNode(YAML::Node node)
		{
//			puts("push");
			m_workingNodes.push(node);
		}


		void PopNode()
		{
//			puts("pop");
			m_workingNodes.pop();
		}

		YAML::Node CurrentNode()
		{
			assert(!m_workingNodes.empty());
			return m_workingNodes.top();
		}


	protected:
		std::vector<YAML::Node>		m_nodes;
		YAML::Node					m_currentNode;
		std::stack<YAML::Node>		m_workingNodes;
		
		// todo: sequence inside sequence, or, map inside map
		YAML::const_iterator		m_sequenceIterator;
		YAML::const_iterator		m_mapIterator;

		// local
		std::map<int64_t, Object*>	m_FileIDToObject;
	};



	class YAMLOutputArchive : public OutputArchive
	{
	public:
		explicit YAMLOutputArchive(std::ostream& fout) : fout(fout)
		{
			fout << "%YAML 1.1\n%TAG !u! tag:unity3d.com,2011:\n";
		}


		void Dump(Object* obj);
		
	protected:
		virtual void Serialize(short t) override				{ fout << t; }
		virtual void Serialize(unsigned short t) override		{ fout << t; }
		virtual void Serialize(int t) override					{ fout << t; }
		virtual void Serialize(unsigned int t) override			{ fout << t; }
		virtual void Serialize(long t) override					{ fout << t; }
		virtual void Serialize(unsigned long t) override		{ fout << t; }
		virtual void Serialize(long long t) override			{ fout << t; }
		virtual void Serialize(unsigned long long t) override	{ fout << t; }
		virtual void Serialize(float t) override				{ fout << t; }
		virtual void Serialize(double t) override				{ fout << t; }
		virtual void Serialize(bool t) override					{ fout << t; }
		virtual void Serialize(std::string const & t) override	{ fout << t; }
		virtual void SerializeNullPtr() override				{ fout << "{fileID: 0}"; }


		//YAMLOutputArchive & operator<<(const Vector2& t) {
		//	fout << Format("{{x:{}, y:{}}}", t.x, t.y);
		//}
		//YAMLOutputArchive & operator<<(const Vector3& t) {
		//	fout << Format("{{x:{}, y:{}, z:{}}}", t.x, t.y, t.x);
		//}
		//YAMLOutputArchive & operator<<(const Vector4& t) {
		//	fout << Format("{{x:{}, y:{}, z:{}, w:{}}}", t.x, t.y, t.x, t.w);
		//}
		//YAMLOutputArchive & operator<<(const Quaternion& t) {
		//	fout << Format("{{x:{}, y:{}, z:{}, w:{}}}", t.x, t.y, t.x, t.w);
		//}

		void MapKey(const char* name) override
		{
			NewLine();
			Indent();
			//state = State::MapKey;

			Serialize(name);

			fout << ": ";
			indent += 2;
			//state = State::MapValue;
			beginOfLine = false;
		}

		void AfterValue() override
		{
			indent -= 2;
			assert(indent >= 0);
			//state = State::None;
			beginOfLine = false;
		}
		
		
		void BeginSequence(int size) override
		{
			if (size == 0)
				fout << "[]";
			else
			{
				NewLine();
			}
			//state = State::Sequence;
		}

		void BeforeSequenceItem() override
		{
			NewLine();
			indent -= 2;
			Indent();
			indent += 2;
			fout << "- ";
		}

		void AfterSequenceItem() override
		{
			beginOfLine = false;
		}
		
		void EndSequence() override
		{
			//state = State::None;
			beginOfLine = true;
		}


		void SerializeObject(Object* t) override;

		void Indent()
		{
			for (int i = 0; i < indent; ++i)
				fout << ' ';
		}

		void NewLine()
		{
			if (!beginOfLine)
			{
				fout << "\n";
				beginOfLine = true;
			}
		}

		std::vector<Object*> todo;
		std::set<Object*> done;
		std::ostream& fout;
		//YAML::Emitter fout;
		int indent = 0;
		
		//enum class State
		//{
		//	None,
		//	Sequence,
		//	MapKey,
		//	MapValue,
		//};
		//// state of next output
		//State state = State::None;

		bool beginOfLine = true;	// at begin of current line? if so, additional "\n" is not needed;
	};
}
