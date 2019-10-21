namespace CounterManagedRoutines
{
  partial class ImageVerifier
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.SubjectImage = new System.Windows.Forms.PictureBox();
      this.TrainingModelLabel = new System.Windows.Forms.Label();
      this.Lib1Pred1Label = new System.Windows.Forms.Label();
      this.ZoomLabel = new System.Windows.Forms.Label();
      this.ZoomFactor = new System.Windows.Forms.ComboBox();
      this.TopPanel = new System.Windows.Forms.Panel();
      this.GroundTruthCountField = new System.Windows.Forms.TextBox();
      this.GroundTruthButton = new System.Windows.Forms.Button();
      this.AddNewClassButtom = new System.Windows.Forms.Button();
      this.OrientationAngle = new System.Windows.Forms.TextBox();
      this.ComputeLengthButton = new System.Windows.Forms.Button();
      this.LengthInMM = new System.Windows.Forms.TextBox();
      this.OriginalClassLabel = new System.Windows.Forms.Label();
      this.OriginalClass = new System.Windows.Forms.TextBox();
      this.UpdateTrainingLibrayButton = new System.Windows.Forms.Button();
      this.VerifiedClass = new System.Windows.Forms.ComboBox();
      this.VerifiedClassLabel = new System.Windows.Forms.Label();
      this.TrainingModelName = new System.Windows.Forms.TextBox();
      this.Lib1Pred1Votes = new System.Windows.Forms.TextBox();
      this.DisplayGridButton = new System.Windows.Forms.Button();
      this.BreakDownTrainingLibrary1Button = new System.Windows.Forms.Button();
      this.ExampleFileName = new System.Windows.Forms.TextBox();
      this.RebuildTrainModelButton = new System.Windows.Forms.Button();
      this.Lib1Pred1Class = new System.Windows.Forms.TextBox();
      this.Lib1Pred1Prob = new System.Windows.Forms.TextBox();
      this.BottomPanel = new System.Windows.Forms.Panel();
      this.OkButton = new System.Windows.Forms.Button();
      ((System.ComponentModel.ISupportInitialize)(this.SubjectImage)).BeginInit();
      this.TopPanel.SuspendLayout();
      this.BottomPanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // SubjectImage
      // 
      this.SubjectImage.Location = new System.Drawing.Point(3, 4);
      this.SubjectImage.Name = "SubjectImage";
      this.SubjectImage.Size = new System.Drawing.Size(503, 364);
      this.SubjectImage.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
      this.SubjectImage.TabIndex = 0;
      this.SubjectImage.TabStop = false;
      this.SubjectImage.MouseClick += new System.Windows.Forms.MouseEventHandler(this.SubjectImage_MouseClick);
      // 
      // TrainingModelLabel
      // 
      this.TrainingModelLabel.AutoSize = true;
      this.TrainingModelLabel.Location = new System.Drawing.Point(7, 138);
      this.TrainingModelLabel.Name = "TrainingModelLabel";
      this.TrainingModelLabel.Size = new System.Drawing.Size(63, 13);
      this.TrainingModelLabel.TabIndex = 3;
      this.TrainingModelLabel.Text = "Train Model";
      // 
      // Lib1Pred1Label
      // 
      this.Lib1Pred1Label.AutoSize = true;
      this.Lib1Pred1Label.Location = new System.Drawing.Point(7, 170);
      this.Lib1Pred1Label.Name = "Lib1Pred1Label";
      this.Lib1Pred1Label.Size = new System.Drawing.Size(54, 13);
      this.Lib1Pred1Label.TabIndex = 7;
      this.Lib1Pred1Label.Text = "Prediction";
      // 
      // ZoomLabel
      // 
      this.ZoomLabel.AutoSize = true;
      this.ZoomLabel.Location = new System.Drawing.Point(7, 9);
      this.ZoomLabel.Name = "ZoomLabel";
      this.ZoomLabel.Size = new System.Drawing.Size(34, 13);
      this.ZoomLabel.TabIndex = 20;
      this.ZoomLabel.Text = "Zoom";
      // 
      // ZoomFactor
      // 
      this.ZoomFactor.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ZoomFactor.FormattingEnabled = true;
      this.ZoomFactor.Items.AddRange(new object[] {
            "10%",
            "25%",
            "50%",
            "100%",
            "200%",
            "400%"});
      this.ZoomFactor.Location = new System.Drawing.Point(74, 6);
      this.ZoomFactor.Name = "ZoomFactor";
      this.ZoomFactor.Size = new System.Drawing.Size(84, 24);
      this.ZoomFactor.TabIndex = 21;
      this.ZoomFactor.Text = "100%";
      this.ZoomFactor.SelectedIndexChanged += new System.EventHandler(this.ZoomFactor_SelectedIndexChanged);
      this.ZoomFactor.Leave += new System.EventHandler(this.ZoomFactor_Leave);
      // 
      // TopPanel
      // 
      this.TopPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.TopPanel.Controls.Add(this.GroundTruthCountField);
      this.TopPanel.Controls.Add(this.GroundTruthButton);
      this.TopPanel.Controls.Add(this.AddNewClassButtom);
      this.TopPanel.Controls.Add(this.OrientationAngle);
      this.TopPanel.Controls.Add(this.ComputeLengthButton);
      this.TopPanel.Controls.Add(this.LengthInMM);
      this.TopPanel.Controls.Add(this.OriginalClassLabel);
      this.TopPanel.Controls.Add(this.OriginalClass);
      this.TopPanel.Controls.Add(this.UpdateTrainingLibrayButton);
      this.TopPanel.Controls.Add(this.VerifiedClass);
      this.TopPanel.Controls.Add(this.VerifiedClassLabel);
      this.TopPanel.Controls.Add(this.TrainingModelName);
      this.TopPanel.Controls.Add(this.Lib1Pred1Votes);
      this.TopPanel.Controls.Add(this.DisplayGridButton);
      this.TopPanel.Controls.Add(this.BreakDownTrainingLibrary1Button);
      this.TopPanel.Controls.Add(this.ExampleFileName);
      this.TopPanel.Controls.Add(this.RebuildTrainModelButton);
      this.TopPanel.Controls.Add(this.ZoomFactor);
      this.TopPanel.Controls.Add(this.ZoomLabel);
      this.TopPanel.Controls.Add(this.TrainingModelLabel);
      this.TopPanel.Controls.Add(this.Lib1Pred1Label);
      this.TopPanel.Controls.Add(this.Lib1Pred1Class);
      this.TopPanel.Controls.Add(this.Lib1Pred1Prob);
      this.TopPanel.Location = new System.Drawing.Point(12, 12);
      this.TopPanel.Name = "TopPanel";
      this.TopPanel.Size = new System.Drawing.Size(511, 286);
      this.TopPanel.TabIndex = 22;
      // 
      // GroundTruthCountField
      // 
      this.GroundTruthCountField.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.GroundTruthCountField.Location = new System.Drawing.Point(49, 250);
      this.GroundTruthCountField.Name = "GroundTruthCountField";
      this.GroundTruthCountField.ReadOnly = true;
      this.GroundTruthCountField.Size = new System.Drawing.Size(60, 22);
      this.GroundTruthCountField.TabIndex = 52;
      this.GroundTruthCountField.TabStop = false;
      this.GroundTruthCountField.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // GroundTruthButton
      // 
      this.GroundTruthButton.Location = new System.Drawing.Point(6, 249);
      this.GroundTruthButton.Name = "GroundTruthButton";
      this.GroundTruthButton.Size = new System.Drawing.Size(35, 23);
      this.GroundTruthButton.TabIndex = 51;
      this.GroundTruthButton.Text = "GT";
      this.GroundTruthButton.UseVisualStyleBackColor = true;
      this.GroundTruthButton.Click += new System.EventHandler(this.GroundTruthButton_Click);
      // 
      // AddNewClassButtom
      // 
      this.AddNewClassButtom.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AddNewClassButtom.Location = new System.Drawing.Point(359, 242);
      this.AddNewClassButtom.Name = "AddNewClassButtom";
      this.AddNewClassButtom.Size = new System.Drawing.Size(140, 30);
      this.AddNewClassButtom.TabIndex = 41;
      this.AddNewClassButtom.Text = "Add New Class";
      this.AddNewClassButtom.UseVisualStyleBackColor = true;
      this.AddNewClassButtom.Click += new System.EventHandler(this.AddNewClassButtom_Click);
      // 
      // OrientationAngle
      // 
      this.OrientationAngle.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.OrientationAngle.Location = new System.Drawing.Point(286, 42);
      this.OrientationAngle.Name = "OrientationAngle";
      this.OrientationAngle.ReadOnly = true;
      this.OrientationAngle.Size = new System.Drawing.Size(90, 22);
      this.OrientationAngle.TabIndex = 40;
      this.OrientationAngle.TabStop = false;
      this.OrientationAngle.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // ComputeLengthButton
      // 
      this.ComputeLengthButton.Location = new System.Drawing.Point(6, 55);
      this.ComputeLengthButton.Name = "ComputeLengthButton";
      this.ComputeLengthButton.Size = new System.Drawing.Size(35, 23);
      this.ComputeLengthButton.TabIndex = 20;
      this.ComputeLengthButton.Text = "CL";
      this.ComputeLengthButton.UseVisualStyleBackColor = true;
      this.ComputeLengthButton.Click += new System.EventHandler(this.ComputeLengthButton_Click);
      // 
      // LengthInMM
      // 
      this.LengthInMM.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.LengthInMM.Location = new System.Drawing.Point(164, 42);
      this.LengthInMM.Name = "LengthInMM";
      this.LengthInMM.ReadOnly = true;
      this.LengthInMM.Size = new System.Drawing.Size(90, 22);
      this.LengthInMM.TabIndex = 38;
      this.LengthInMM.TabStop = false;
      this.LengthInMM.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // OriginalClassLabel
      // 
      this.OriginalClassLabel.AutoSize = true;
      this.OriginalClassLabel.Location = new System.Drawing.Point(10, 100);
      this.OriginalClassLabel.Name = "OriginalClassLabel";
      this.OriginalClassLabel.Size = new System.Drawing.Size(32, 13);
      this.OriginalClassLabel.TabIndex = 37;
      this.OriginalClassLabel.Text = "Class";
      // 
      // OriginalClass
      // 
      this.OriginalClass.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.OriginalClass.Location = new System.Drawing.Point(76, 96);
      this.OriginalClass.Name = "OriginalClass";
      this.OriginalClass.ReadOnly = true;
      this.OriginalClass.Size = new System.Drawing.Size(423, 22);
      this.OriginalClass.TabIndex = 36;
      this.OriginalClass.TabStop = false;
      // 
      // UpdateTrainingLibrayButton
      // 
      this.UpdateTrainingLibrayButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.UpdateTrainingLibrayButton.Location = new System.Drawing.Point(184, 242);
      this.UpdateTrainingLibrayButton.Name = "UpdateTrainingLibrayButton";
      this.UpdateTrainingLibrayButton.Size = new System.Drawing.Size(140, 30);
      this.UpdateTrainingLibrayButton.TabIndex = 35;
      this.UpdateTrainingLibrayButton.Text = "Update Library";
      this.UpdateTrainingLibrayButton.UseVisualStyleBackColor = true;
      this.UpdateTrainingLibrayButton.Click += new System.EventHandler(this.UpdateTrainingLibrayButton_Click);
      // 
      // VerifiedClass
      // 
      this.VerifiedClass.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.VerifiedClass.FormattingEnabled = true;
      this.VerifiedClass.Location = new System.Drawing.Point(76, 204);
      this.VerifiedClass.Name = "VerifiedClass";
      this.VerifiedClass.Size = new System.Drawing.Size(423, 26);
      this.VerifiedClass.TabIndex = 50;
      this.VerifiedClass.SelectedIndexChanged += new System.EventHandler(this.VerifiedClass_SelectedIndexChanged);
      // 
      // VerifiedClassLabel
      // 
      this.VerifiedClassLabel.AutoSize = true;
      this.VerifiedClassLabel.Location = new System.Drawing.Point(7, 207);
      this.VerifiedClassLabel.Name = "VerifiedClassLabel";
      this.VerifiedClassLabel.Size = new System.Drawing.Size(32, 13);
      this.VerifiedClassLabel.TabIndex = 33;
      this.VerifiedClassLabel.Text = "Class";
      // 
      // TrainingModelName
      // 
      this.TrainingModelName.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TrainingModelName.Location = new System.Drawing.Point(76, 137);
      this.TrainingModelName.Name = "TrainingModelName";
      this.TrainingModelName.ReadOnly = true;
      this.TrainingModelName.Size = new System.Drawing.Size(289, 22);
      this.TrainingModelName.TabIndex = 32;
      this.TrainingModelName.TabStop = false;
      // 
      // Lib1Pred1Votes
      // 
      this.Lib1Pred1Votes.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib1Pred1Votes.Location = new System.Drawing.Point(457, 166);
      this.Lib1Pred1Votes.Name = "Lib1Pred1Votes";
      this.Lib1Pred1Votes.ReadOnly = true;
      this.Lib1Pred1Votes.Size = new System.Drawing.Size(42, 22);
      this.Lib1Pred1Votes.TabIndex = 30;
      this.Lib1Pred1Votes.TabStop = false;
      this.Lib1Pred1Votes.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // DisplayGridButton
      // 
      this.DisplayGridButton.Location = new System.Drawing.Point(6, 26);
      this.DisplayGridButton.Name = "DisplayGridButton";
      this.DisplayGridButton.Size = new System.Drawing.Size(35, 23);
      this.DisplayGridButton.TabIndex = 10;
      this.DisplayGridButton.Text = "Grid";
      this.DisplayGridButton.UseVisualStyleBackColor = true;
      this.DisplayGridButton.Click += new System.EventHandler(this.DisplayGridButton_Click);
      // 
      // BreakDownTrainingLibrary1Button
      // 
      this.BreakDownTrainingLibrary1Button.Location = new System.Drawing.Point(371, 134);
      this.BreakDownTrainingLibrary1Button.Name = "BreakDownTrainingLibrary1Button";
      this.BreakDownTrainingLibrary1Button.Size = new System.Drawing.Size(71, 23);
      this.BreakDownTrainingLibrary1Button.TabIndex = 30;
      this.BreakDownTrainingLibrary1Button.TabStop = false;
      this.BreakDownTrainingLibrary1Button.Text = "BreakDown";
      this.BreakDownTrainingLibrary1Button.UseVisualStyleBackColor = true;
      this.BreakDownTrainingLibrary1Button.Click += new System.EventHandler(this.BreakDownTrainingLibrary1Button_Click);
      // 
      // ExampleFileName
      // 
      this.ExampleFileName.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ExampleFileName.Location = new System.Drawing.Point(164, 6);
      this.ExampleFileName.Name = "ExampleFileName";
      this.ExampleFileName.ReadOnly = true;
      this.ExampleFileName.Size = new System.Drawing.Size(335, 22);
      this.ExampleFileName.TabIndex = 26;
      this.ExampleFileName.TabStop = false;
      // 
      // RebuildTrainModelButton
      // 
      this.RebuildTrainModelButton.Location = new System.Drawing.Point(448, 134);
      this.RebuildTrainModelButton.Name = "RebuildTrainModelButton";
      this.RebuildTrainModelButton.Size = new System.Drawing.Size(51, 23);
      this.RebuildTrainModelButton.TabIndex = 35;
      this.RebuildTrainModelButton.TabStop = false;
      this.RebuildTrainModelButton.Text = "Rebuild";
      this.RebuildTrainModelButton.UseVisualStyleBackColor = true;
      this.RebuildTrainModelButton.Click += new System.EventHandler(this.RebuildTrainLibrary1Button_Click);
      // 
      // Lib1Pred1Class
      // 
      this.Lib1Pred1Class.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib1Pred1Class.Location = new System.Drawing.Point(74, 167);
      this.Lib1Pred1Class.Name = "Lib1Pred1Class";
      this.Lib1Pred1Class.ReadOnly = true;
      this.Lib1Pred1Class.Size = new System.Drawing.Size(312, 22);
      this.Lib1Pred1Class.TabIndex = 8;
      this.Lib1Pred1Class.TabStop = false;
      // 
      // Lib1Pred1Prob
      // 
      this.Lib1Pred1Prob.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib1Pred1Prob.Location = new System.Drawing.Point(392, 166);
      this.Lib1Pred1Prob.Name = "Lib1Pred1Prob";
      this.Lib1Pred1Prob.ReadOnly = true;
      this.Lib1Pred1Prob.Size = new System.Drawing.Size(60, 22);
      this.Lib1Pred1Prob.TabIndex = 9;
      this.Lib1Pred1Prob.TabStop = false;
      this.Lib1Pred1Prob.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // BottomPanel
      // 
      this.BottomPanel.AutoScroll = true;
      this.BottomPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.BottomPanel.Controls.Add(this.SubjectImage);
      this.BottomPanel.Location = new System.Drawing.Point(12, 311);
      this.BottomPanel.Name = "BottomPanel";
      this.BottomPanel.Size = new System.Drawing.Size(511, 374);
      this.BottomPanel.TabIndex = 23;
      this.BottomPanel.SizeChanged += new System.EventHandler(this.LeftPanel_SizeChanged);
      // 
      // OkButton
      // 
      this.OkButton.Location = new System.Drawing.Point(231, 694);
      this.OkButton.Name = "OkButton";
      this.OkButton.Size = new System.Drawing.Size(75, 23);
      this.OkButton.TabIndex = 24;
      this.OkButton.Text = "OK";
      this.OkButton.UseVisualStyleBackColor = true;
      this.OkButton.Click += new System.EventHandler(this.OkButton_Click);
      // 
      // ImageVerifier
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(534, 719);
      this.Controls.Add(this.OkButton);
      this.Controls.Add(this.BottomPanel);
      this.Controls.Add(this.TopPanel);
      this.MinimumSize = new System.Drawing.Size(550, 585);
      this.Name = "ImageVerifier";
      this.Text = "Image Verifier";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ImageVerifier_FormClosing);
      this.Load += new System.EventHandler(this.ImageVerifier_Load);
      this.Resize += new System.EventHandler(this.ImageVerifier_Resize);
      ((System.ComponentModel.ISupportInitialize)(this.SubjectImage)).EndInit();
      this.TopPanel.ResumeLayout(false);
      this.TopPanel.PerformLayout();
      this.BottomPanel.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.PictureBox SubjectImage;
    private System.Windows.Forms.Label TrainingModelLabel;
    private System.Windows.Forms.Label Lib1Pred1Label;
    private System.Windows.Forms.Label ZoomLabel;
    private System.Windows.Forms.ComboBox ZoomFactor;
    private System.Windows.Forms.Panel TopPanel;
    private System.Windows.Forms.Panel BottomPanel;
    private System.Windows.Forms.Button RebuildTrainModelButton;
    private System.Windows.Forms.TextBox ExampleFileName;
    private System.Windows.Forms.Button BreakDownTrainingLibrary1Button;
    private System.Windows.Forms.Button DisplayGridButton;
    private System.Windows.Forms.TextBox Lib1Pred1Votes;
    private System.Windows.Forms.TextBox TrainingModelName;
    private System.Windows.Forms.Button UpdateTrainingLibrayButton;
    private System.Windows.Forms.ComboBox VerifiedClass;
    private System.Windows.Forms.Label VerifiedClassLabel;
    private System.Windows.Forms.Label OriginalClassLabel;
    private System.Windows.Forms.TextBox OriginalClass;
    private System.Windows.Forms.TextBox Lib1Pred1Class;
    private System.Windows.Forms.Button OkButton;
    private System.Windows.Forms.TextBox LengthInMM;
    private System.Windows.Forms.TextBox Lib1Pred1Prob;
    private System.Windows.Forms.Button ComputeLengthButton;
    private System.Windows.Forms.TextBox OrientationAngle;
    private System.Windows.Forms.Button AddNewClassButtom;
    private System.Windows.Forms.TextBox GroundTruthCountField;
    private System.Windows.Forms.Button GroundTruthButton;
  }
}