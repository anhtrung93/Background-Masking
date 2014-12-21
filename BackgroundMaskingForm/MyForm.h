#pragma once
#include<cstdio>
#include<cstring>
#include<iostream>
#include<cstdlib>

namespace BackgroundMaskingForm {

	using namespace std;

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;



	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::NumericUpDown^  widthNumeric;
	private: System::Windows::Forms::NumericUpDown^  heightNumeric;
	private: System::Windows::Forms::NumericUpDown^  minDisNumeric;
	private: System::Windows::Forms::NumericUpDown^  maxDisNumeric;

	private: System::Windows::Forms::NumericUpDown^  stepDisNumeric;
	private: System::Windows::Forms::NumericUpDown^  blockSizeNumeric;

	private: System::Windows::Forms::NumericUpDown^  minSegmentAreaNumeric;
	private: System::Windows::Forms::NumericUpDown^  lowColorThresNumeric;
	private: System::Windows::Forms::NumericUpDown^  highColorThresNumeric;

	private: System::Windows::Forms::TextBox^  smoothEffText;
	private: System::Windows::Forms::TextBox^  smoothEff2Text;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::NumericUpDown^  foregndLowThresNumeric;
	private: System::Windows::Forms::NumericUpDown^  foregndHighThres;
	private: System::Windows::Forms::Label^  label10;

	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::TextBox^  leftImageTextBox;
	private: System::Windows::Forms::TextBox^  leftBackgroundTextBox;





	private: System::Windows::Forms::Label^  label17;
	private: System::Windows::Forms::TextBox^  outputTextBox;
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::PictureBox^  pictureBox2;
	private: System::Windows::Forms::Button^  browseLeftImage;
	private: System::Windows::Forms::OpenFileDialog^  openLeftImage;
	private: System::Windows::Forms::OpenFileDialog^  openLeftBackground;
	private: System::Windows::Forms::Button^  browseLeftBackground;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  segmentButton;
	private: System::Windows::Forms::Button^  button4;




















	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->widthNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->heightNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->minDisNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->maxDisNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->stepDisNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->blockSizeNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->minSegmentAreaNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->lowColorThresNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->highColorThresNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->smoothEffText = (gcnew System::Windows::Forms::TextBox());
			this->smoothEff2Text = (gcnew System::Windows::Forms::TextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->foregndLowThresNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->foregndHighThres = (gcnew System::Windows::Forms::NumericUpDown());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->leftImageTextBox = (gcnew System::Windows::Forms::TextBox());
			this->leftBackgroundTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->outputTextBox = (gcnew System::Windows::Forms::TextBox());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->browseLeftImage = (gcnew System::Windows::Forms::Button());
			this->openLeftImage = (gcnew System::Windows::Forms::OpenFileDialog());
			this->openLeftBackground = (gcnew System::Windows::Forms::OpenFileDialog());
			this->browseLeftBackground = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->segmentButton = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->widthNumeric))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->heightNumeric))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->minDisNumeric))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maxDisNumeric))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->stepDisNumeric))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->blockSizeNumeric))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->minSegmentAreaNumeric))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->lowColorThresNumeric))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->highColorThresNumeric))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->foregndLowThresNumeric))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->foregndHighThres))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->BeginInit();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(532, 467);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(83, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"All";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(421, 11);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(44, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"WIDTH";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(421, 38);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(48, 13);
			this->label2->TabIndex = 2;
			this->label2->Text = L"HEIGHT";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(421, 65);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(67, 13);
			this->label3->TabIndex = 3;
			this->label3->Text = L"Min Disparity";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(421, 91);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(70, 13);
			this->label4->TabIndex = 4;
			this->label4->Text = L"Max Disparity";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(421, 119);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(72, 13);
			this->label5->TabIndex = 5;
			this->label5->Text = L"Step Disparity";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(421, 145);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(56, 13);
			this->label6->TabIndex = 6;
			this->label6->Text = L"SmoothEff";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(421, 171);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(62, 13);
			this->label7->TabIndex = 7;
			this->label7->Text = L"SmoothEff2";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(421, 199);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(78, 13);
			this->label11->TabIndex = 11;
			this->label11->Text = L"LowColorThres";
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(421, 227);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(80, 13);
			this->label12->TabIndex = 12;
			this->label12->Text = L"HighColorThres";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(421, 254);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(88, 13);
			this->label13->TabIndex = 13;
			this->label13->Text = L"MinSegmentArea";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(421, 280);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(54, 13);
			this->label14->TabIndex = 14;
			this->label14->Text = L"BlockSize";
			// 
			// widthNumeric
			// 
			this->widthNumeric->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 224, 0, 0, 0 });
			this->widthNumeric->Location = System::Drawing::Point(515, 9);
			this->widthNumeric->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3584, 0, 0, 0 });
			this->widthNumeric->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 224, 0, 0, 0 });
			this->widthNumeric->Name = L"widthNumeric";
			this->widthNumeric->Size = System::Drawing::Size(102, 20);
			this->widthNumeric->TabIndex = 29;
			this->widthNumeric->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 896, 0, 0, 0 });
			// 
			// heightNumeric
			// 
			this->heightNumeric->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 126, 0, 0, 0 });
			this->heightNumeric->Location = System::Drawing::Point(515, 36);
			this->heightNumeric->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2016, 0, 0, 0 });
			this->heightNumeric->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 126, 0, 0, 0 });
			this->heightNumeric->Name = L"heightNumeric";
			this->heightNumeric->Size = System::Drawing::Size(102, 20);
			this->heightNumeric->TabIndex = 30;
			this->heightNumeric->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 504, 0, 0, 0 });
			// 
			// minDisNumeric
			// 
			this->minDisNumeric->Location = System::Drawing::Point(515, 63);
			this->minDisNumeric->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3584, 0, 0, 0 });
			this->minDisNumeric->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3584, 0, 0, System::Int32::MinValue });
			this->minDisNumeric->Name = L"minDisNumeric";
			this->minDisNumeric->Size = System::Drawing::Size(102, 20);
			this->minDisNumeric->TabIndex = 31;
			this->minDisNumeric->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 40, 0, 0, System::Int32::MinValue });
			// 
			// maxDisNumeric
			// 
			this->maxDisNumeric->Location = System::Drawing::Point(515, 89);
			this->maxDisNumeric->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3584, 0, 0, 0 });
			this->maxDisNumeric->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3584, 0, 0, System::Int32::MinValue });
			this->maxDisNumeric->Name = L"maxDisNumeric";
			this->maxDisNumeric->Size = System::Drawing::Size(102, 20);
			this->maxDisNumeric->TabIndex = 32;
			this->maxDisNumeric->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 80, 0, 0, 0 });
			// 
			// stepDisNumeric
			// 
			this->stepDisNumeric->Location = System::Drawing::Point(515, 117);
			this->stepDisNumeric->Name = L"stepDisNumeric";
			this->stepDisNumeric->Size = System::Drawing::Size(102, 20);
			this->stepDisNumeric->TabIndex = 33;
			this->stepDisNumeric->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
			// 
			// blockSizeNumeric
			// 
			this->blockSizeNumeric->Location = System::Drawing::Point(515, 278);
			this->blockSizeNumeric->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 99, 0, 0, 0 });
			this->blockSizeNumeric->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->blockSizeNumeric->Name = L"blockSizeNumeric";
			this->blockSizeNumeric->Size = System::Drawing::Size(102, 20);
			this->blockSizeNumeric->TabIndex = 36;
			this->blockSizeNumeric->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3, 0, 0, 0 });
			// 
			// minSegmentAreaNumeric
			// 
			this->minSegmentAreaNumeric->Location = System::Drawing::Point(515, 252);
			this->minSegmentAreaNumeric->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
			this->minSegmentAreaNumeric->Name = L"minSegmentAreaNumeric";
			this->minSegmentAreaNumeric->Size = System::Drawing::Size(102, 20);
			this->minSegmentAreaNumeric->TabIndex = 37;
			this->minSegmentAreaNumeric->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 30, 0, 0, 0 });
			// 
			// lowColorThresNumeric
			// 
			this->lowColorThresNumeric->Location = System::Drawing::Point(515, 197);
			this->lowColorThresNumeric->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 500, 0, 0, 0 });
			this->lowColorThresNumeric->Name = L"lowColorThresNumeric";
			this->lowColorThresNumeric->Size = System::Drawing::Size(102, 20);
			this->lowColorThresNumeric->TabIndex = 38;
			this->lowColorThresNumeric->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3, 0, 0, 0 });
			// 
			// highColorThresNumeric
			// 
			this->highColorThresNumeric->Location = System::Drawing::Point(515, 225);
			this->highColorThresNumeric->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 500, 0, 0, 0 });
			this->highColorThresNumeric->Name = L"highColorThresNumeric";
			this->highColorThresNumeric->Size = System::Drawing::Size(102, 20);
			this->highColorThresNumeric->TabIndex = 39;
			this->highColorThresNumeric->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 200, 0, 0, 0 });
			// 
			// smoothEffText
			// 
			this->smoothEffText->Location = System::Drawing::Point(515, 142);
			this->smoothEffText->Name = L"smoothEffText";
			this->smoothEffText->Size = System::Drawing::Size(100, 20);
			this->smoothEffText->TabIndex = 43;
			this->smoothEffText->Text = L"2.0";
			// 
			// smoothEff2Text
			// 
			this->smoothEff2Text->Location = System::Drawing::Point(515, 168);
			this->smoothEff2Text->Name = L"smoothEff2Text";
			this->smoothEff2Text->Size = System::Drawing::Size(100, 20);
			this->smoothEff2Text->TabIndex = 44;
			this->smoothEff2Text->Text = L"0.3";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(421, 306);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(93, 13);
			this->label8->TabIndex = 45;
			this->label8->Text = L"ForegndLowThres";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(421, 330);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(95, 13);
			this->label9->TabIndex = 46;
			this->label9->Text = L"ForegndHighThres";
			// 
			// foregndLowThresNumeric
			// 
			this->foregndLowThresNumeric->Location = System::Drawing::Point(515, 304);
			this->foregndLowThresNumeric->Name = L"foregndLowThresNumeric";
			this->foregndLowThresNumeric->Size = System::Drawing::Size(102, 20);
			this->foregndLowThresNumeric->TabIndex = 47;
			// 
			// foregndHighThres
			// 
			this->foregndHighThres->Location = System::Drawing::Point(515, 328);
			this->foregndHighThres->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 200, 0, 0, 0 });
			this->foregndHighThres->Name = L"foregndHighThres";
			this->foregndHighThres->Size = System::Drawing::Size(102, 20);
			this->foregndHighThres->TabIndex = 48;
			this->foregndHighThres->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 40, 0, 0, 0 });
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(420, 356);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(54, 13);
			this->label10->TabIndex = 49;
			this->label10->Text = L"LeftImage";
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(421, 412);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(68, 13);
			this->label16->TabIndex = 51;
			this->label16->Text = L"OutputFolder";
			// 
			// leftImageTextBox
			// 
			this->leftImageTextBox->Location = System::Drawing::Point(515, 353);
			this->leftImageTextBox->Name = L"leftImageTextBox";
			this->leftImageTextBox->Size = System::Drawing::Size(75, 20);
			this->leftImageTextBox->TabIndex = 52;
			this->leftImageTextBox->Text = L"E:\\Dropbox\\Computer Vision Topic - Background Masking\\Images data\\JPEGs\\DSCF7459-"
				L"L.jpg";
			// 
			// leftBackgroundTextBox
			// 
			this->leftBackgroundTextBox->Location = System::Drawing::Point(515, 380);
			this->leftBackgroundTextBox->Name = L"leftBackgroundTextBox";
			this->leftBackgroundTextBox->Size = System::Drawing::Size(75, 20);
			this->leftBackgroundTextBox->TabIndex = 54;
			this->leftBackgroundTextBox->Text = L"E:\\Dropbox\\Computer Vision Topic - Background Masking\\Images data\\JPEGs\\DSCF7482-"
				L"L.jpg";
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Location = System::Drawing::Point(421, 383);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(83, 13);
			this->label17->TabIndex = 55;
			this->label17->Text = L"LeftBackground";
			// 
			// outputTextBox
			// 
			this->outputTextBox->Location = System::Drawing::Point(515, 409);
			this->outputTextBox->Name = L"outputTextBox";
			this->outputTextBox->Size = System::Drawing::Size(100, 20);
			this->outputTextBox->TabIndex = 56;
			this->outputTextBox->Text = L"E:\\Temp\\DSCF7459\\";
			// 
			// pictureBox1
			// 
			this->pictureBox1->Location = System::Drawing::Point(12, 9);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(386, 216);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox1->TabIndex = 57;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->UseWaitCursor = true;
			// 
			// pictureBox2
			// 
			this->pictureBox2->Location = System::Drawing::Point(13, 232);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(385, 220);
			this->pictureBox2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox2->TabIndex = 58;
			this->pictureBox2->TabStop = false;
			this->pictureBox2->UseWaitCursor = true;
			// 
			// browseLeftImage
			// 
			this->browseLeftImage->Location = System::Drawing::Point(590, 352);
			this->browseLeftImage->Name = L"browseLeftImage";
			this->browseLeftImage->Size = System::Drawing::Size(27, 21);
			this->browseLeftImage->TabIndex = 59;
			this->browseLeftImage->Text = L"...";
			this->browseLeftImage->UseVisualStyleBackColor = true;
			this->browseLeftImage->Click += gcnew System::EventHandler(this, &MyForm::browseLeftImage_Click);
			// 
			// openLeftImage
			// 
			this->openLeftImage->FileName = L"openFileDialog1";
			this->openLeftImage->Title = L"Please select left image (ends with \"*-L.jpg\")";
			// 
			// openLeftBackground
			// 
			this->openLeftBackground->FileName = L"openFileDialog1";
			this->openLeftBackground->Title = L"Please select left background (ends with \"*-L.jpg\")";
			// 
			// browseLeftBackground
			// 
			this->browseLeftBackground->Location = System::Drawing::Point(590, 380);
			this->browseLeftBackground->Name = L"browseLeftBackground";
			this->browseLeftBackground->Size = System::Drawing::Size(27, 21);
			this->browseLeftBackground->TabIndex = 60;
			this->browseLeftBackground->Text = L"...";
			this->browseLeftBackground->UseVisualStyleBackColor = true;
			this->browseLeftBackground->Click += gcnew System::EventHandler(this, &MyForm::browseLeftBackground_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(532, 439);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(83, 23);
			this->button2->TabIndex = 61;
			this->button2->Text = L"GraphCut";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// segmentButton
			// 
			this->segmentButton->Location = System::Drawing::Point(439, 439);
			this->segmentButton->Name = L"segmentButton";
			this->segmentButton->Size = System::Drawing::Size(89, 23);
			this->segmentButton->TabIndex = 62;
			this->segmentButton->Text = L"Segmentation";
			this->segmentButton->UseVisualStyleBackColor = true;
			this->segmentButton->Click += gcnew System::EventHandler(this, &MyForm::segmentButton_Click);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(439, 467);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(89, 23);
			this->button4->TabIndex = 63;
			this->button4->Text = L"PlaneFitting";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &MyForm::button4_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(629, 503);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->segmentButton);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->browseLeftBackground);
			this->Controls->Add(this->browseLeftImage);
			this->Controls->Add(this->pictureBox2);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->outputTextBox);
			this->Controls->Add(this->label17);
			this->Controls->Add(this->leftBackgroundTextBox);
			this->Controls->Add(this->leftImageTextBox);
			this->Controls->Add(this->label16);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->foregndHighThres);
			this->Controls->Add(this->foregndLowThresNumeric);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->smoothEff2Text);
			this->Controls->Add(this->smoothEffText);
			this->Controls->Add(this->highColorThresNumeric);
			this->Controls->Add(this->lowColorThresNumeric);
			this->Controls->Add(this->minSegmentAreaNumeric);
			this->Controls->Add(this->blockSizeNumeric);
			this->Controls->Add(this->stepDisNumeric);
			this->Controls->Add(this->maxDisNumeric);
			this->Controls->Add(this->minDisNumeric);
			this->Controls->Add(this->heightNumeric);
			this->Controls->Add(this->widthNumeric);
			this->Controls->Add(this->label14);
			this->Controls->Add(this->label13);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button1);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->widthNumeric))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->heightNumeric))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->minDisNumeric))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maxDisNumeric))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->stepDisNumeric))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->blockSizeNumeric))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->minSegmentAreaNumeric))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->lowColorThresNumeric))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->highColorThresNumeric))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->foregndLowThresNumeric))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->foregndHighThres))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void createConfigFile(){
				 const char OUTPUT[] = "..\\Release\\ConfigFile.txt";
				 FILE * temp = freopen(OUTPUT, "w", stdout);
				 printf("DEFAULT_WIDTH\t\t\t%s\n", this->widthNumeric->Text);
				 printf("DEFAULT_HEIGHT\t\t\t%s\n", this->heightNumeric->Text);
				 printf("MIN_DISPARITY\t\t\t%s\n", this->minDisNumeric->Text);
				 printf("MAX_DISPARITY\t\t\t%s\n", this->maxDisNumeric->Text);
				 printf("STEP_DISPARITY\t\t\t%s\n", this->stepDisNumeric->Text);
				 printf("SMOOTH_EFFICIENT\t\t\t%s\n", this->smoothEffText->Text);
				 printf("SMOOTH_EFFICIENT2\t\t\t%s\n", this->smoothEff2Text->Text);
				 printf("LOWER_COLOR_THRESHOLD\t\t\t%s\n", this->lowColorThresNumeric->Text);
				 printf("UPPER_COLOR_THRESHOLD\t\t\t%s\n", this->highColorThresNumeric->Text);
				 printf("MIN_SEGMENT_AREA\t\t\t%s\n", this->minSegmentAreaNumeric->Text);
				 printf("BLOCK_SIZE\t\t\t%s\n", this->blockSizeNumeric->Text);
				 printf("LOW_THRES\t\t\t%s\n", this->foregndLowThresNumeric->Text);
				 printf("HIGH_THRES\t\t\t%s\n", this->foregndHighThres->Text);
				 printf("LEFT_IMAGE\t\t\t%s\n", this->leftImageTextBox->Text);
				 printf("LEFT_BACKGROUND\t\t\t%s\n", this->leftBackgroundTextBox->Text);
				 printf("STORE_PATH\t\t\t%s\n", this->outputTextBox->Text);
				 fclose(temp);
	}

	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->removeImages();

				 this->createConfigFile();

				 system("..\\Release\\BackgroundMasking.exe ..\\Release\\ConfigFile.txt");

				 System::String^ name = this->leftImageTextBox->Text->Substring(this->leftImageTextBox->Text->Length - 14, 8);
				 System::String^ fileLeft = this->outputTextBox->Text + name + "-maskL.jpg";
				 System::String^ fileRight = this->outputTextBox->Text + name + "-maskR.jpg";
				 this->showImages(fileLeft, fileRight);
	}

	private: System::Void browseLeftImage_Click(System::Object^  sender, System::EventArgs^  e) {
				 System::Windows::Forms::DialogResult result = this->openLeftImage->ShowDialog();
				 if (result == System::Windows::Forms::DialogResult::OK){
					 System::String^ file = this->openLeftImage->FileName;
					 this->leftImageTextBox->Text = file;

					 removeImages();
				 }
	}

	private: System::Void browseLeftBackground_Click(System::Object^  sender, System::EventArgs^  e) {
				 System::Windows::Forms::DialogResult result = this->openLeftBackground->ShowDialog();
				 if (result == System::Windows::Forms::DialogResult::OK){
					 System::String^ file = this->openLeftBackground->FileName;
					 this->leftBackgroundTextBox->Text = file;
				 }
	}

	private: System::Void removeImages(){
				 if (this->pictureBox1->Image != nullptr){
					 delete this->pictureBox1->Image;
				 }
				 if (this->pictureBox2->Image != nullptr){
					 delete this->pictureBox2->Image;
				 }
				 showImages(this->leftImageTextBox->Text, this->leftImageTextBox->Text->Substring(0, this->leftImageTextBox->Text->Length - 6) + "-R.jpg");
	}

	private: System::Void showImages(System::String^ fileLeft, System::String^ fileRight){
				 try {
					 this->pictureBox1->Image = Image::FromFile(fileLeft);
					 this->pictureBox2->Image = Image::FromFile(fileRight);
				 }
				 catch (...){
					 this->pictureBox1->Image = nullptr;
					 this->pictureBox2->Image = nullptr;
				 }
	}
	private: System::Void segmentButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 removeImages();

				 this->createConfigFile();


				 system("..\\Release\\BackgroundMasking.exe segment ..\\Release\\ConfigFile.txt");

				 System::String^ name = this->leftImageTextBox->Text->Substring(this->leftImageTextBox->Text->Length - 14, 8);
				 System::String^ fileLeft = this->outputTextBox->Text + name + "-segmented-L.jpg";
				 System::String^ fileRight = this->outputTextBox->Text + name + "-segmented-R.jpg";
				 this->showImages(fileLeft, fileRight);
	}
	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
				 removeImages();

				 this->createConfigFile();

				 system("..\\Release\\BackgroundMasking.exe graphcut ..\\Release\\ConfigFile.txt");

				 System::String^ name = this->leftImageTextBox->Text->Substring(this->leftImageTextBox->Text->Length - 14, 8);
				 System::String^ fileLeft = this->outputTextBox->Text + name + "-cutL.jpg";
				 System::String^ fileRight = this->outputTextBox->Text + name + "-cutR.jpg";
				 this->showImages(fileLeft, fileRight);
	}
	private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {
				 removeImages();

				 this->createConfigFile();

				 system("..\\Release\\BackgroundMasking.exe planefitting ..\\Release\\ConfigFile.txt");

				 System::String^ name = this->leftImageTextBox->Text->Substring(this->leftImageTextBox->Text->Length - 14, 8);
				 System::String^ fileLeft = this->outputTextBox->Text + name + "-fitL.jpg";
				 System::String^ fileRight = this->outputTextBox->Text + name + "-fitR.jpg";
				 this->showImages(fileLeft, fileRight);
	}
};
}
