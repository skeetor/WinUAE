#pragma once

#include "res/resource.h"

class OptionsPage1 : public CMFCPropertyPage
{
	DECLARE_DYNCREATE(OptionsPage1)

public:
	OptionsPage1();
	~OptionsPage1();

	enum { IDD = IDD_OPTIONSPAGE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

class OptionsPage12 : public CMFCPropertyPage
{
	DECLARE_DYNCREATE(OptionsPage12)

public:
	OptionsPage12();
	~OptionsPage12();

	enum { IDD = IDD_OPTIONSPAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

class OptionsPage21 : public CMFCPropertyPage
{
	DECLARE_DYNCREATE(OptionsPage21)

public:
	OptionsPage21();
	~OptionsPage21();

	enum { IDD = IDD_OPTIONSPAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

class OptionsPage22 : public CMFCPropertyPage
{
	DECLARE_DYNCREATE(OptionsPage22)

public:
	OptionsPage22();
	~OptionsPage22();

	enum { IDD = IDD_OPTIONSPAGE4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

class OptionsPage31 : public CMFCPropertyPage
{
	DECLARE_DYNCREATE(OptionsPage31)

public:
	OptionsPage31();
	~OptionsPage31();

	enum { IDD = IDD_OPTIONSPAGE5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

class OptionsPage32 : public CMFCPropertyPage
{
	DECLARE_DYNCREATE(OptionsPage32)

public:
	OptionsPage32();
	~OptionsPage32();

	enum { IDD = IDD_OPTIONSPAGE6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

