namespace KKManagedRoutines
{
  partial class ImageViewer
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
      this.PlanktonImage = new System.Windows.Forms.PictureBox ();
      this.TrainingLibrary1Label = new System.Windows.Forms.Label ();
      this.TrainingLibrary1 = new System.Windows.Forms.ComboBox ();
      this.TrainingLibrary2 = new System.Windows.Forms.ComboBox ();
      this.TrainingLibrary2Label = new System.Windows.Forms.Label ();
      this.Lib1Pred1Label = new System.Windows.Forms.Label ();
      this.Lib1Pred1Class = new System.Windows.Forms.TextBox ();
      this.Lib1Pred1Prob = new System.Windows.Forms.TextBox ();
      this.Lib1Pred2Prob = new System.Windows.Forms.TextBox ();
      this.Lib1Pred2Class = new System.Windows.Forms.TextBox ();
      this.Lib1Pred2Label = new System.Windows.Forms.Label ();
      this.Lib2Pred2Prob = new System.Windows.Forms.TextBox ();
      this.Lib2Pred2Class = new System.Windows.Forms.TextBox ();
      this.Lib2Pred2Label = new System.Windows.Forms.Label ();
      this.Lib2Pred1Prob = new System.Windows.Forms.TextBox ();
      this.Lib2Pred1Class = new System.Windows.Forms.TextBox ();
      this.Lib2Pred1Label = new System.Windows.Forms.Label ();
      this.ZoomLabel = new System.Windows.Forms.Label ();
      this.ZoomFactor = new System.Windows.Forms.ComboBox ();
      this.RightPanel = new System.Windows.Forms.Panel ();
      this.Lib2Pred2Votes = new System.Windows.Forms.TextBox ();
      this.Lib2Pred1Votes = new System.Windows.Forms.TextBox ();
      this.Lib1Pred2Votes = new System.Windows.Forms.TextBox ();
      this.Lib1Pred1Votes = new System.Windows.Forms.TextBox ();
      this.DisplayGridButton = new System.Windows.Forms.Button ();
      this.BreakDownTrainingLibrary2Button = new System.Windows.Forms.Button ();
      this.BreakDownTrainingLibrary1Button = new System.Windows.Forms.Button ();
      this.ExampleFileName = new System.Windows.Forms.TextBox ();
      this.RebuildTrainLibrary2Button = new System.Windows.Forms.Button ();
      this.RebuildTrainLibrary1Button = new System.Windows.Forms.Button ();
      this.LeftPanel = new System.Windows.Forms.Panel ();
      ((System.ComponentModel.ISupportInitialize)(this.PlanktonImage)).BeginInit ();
      this.RightPanel.SuspendLayout ();
      this.LeftPanel.SuspendLayout ();
      this.SuspendLayout ();
      // 
      // PlanktonImage
      // 
      this.PlanktonImage.Location = new System.Drawing.Point (3, 4);
      this.PlanktonImage.Name = "PlanktonImage";
      this.PlanktonImage.Size = new System.Drawing.Size (971, 455);
      this.PlanktonImage.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
      this.PlanktonImage.TabIndex = 0;
      this.PlanktonImage.TabStop = false;
      // 
      // TrainingLibrary1Label
      // 
      this.TrainingLibrary1Label.AutoSize = true;
      this.TrainingLibrary1Label.Location = new System.Drawing.Point (7, 59);
      this.TrainingLibrary1Label.Name = "TrainingLibrary1Label";
      this.TrainingLibrary1Label.Size = new System.Drawing.Size (62, 13);
      this.TrainingLibrary1Label.TabIndex = 3;
      this.TrainingLibrary1Label.Text = "TrainLibrary";
      // 
      // TrainingLibrary1
      // 
      this.TrainingLibrary1.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TrainingLibrary1.FormattingEnabled = true;
      this.TrainingLibrary1.Location = new System.Drawing.Point (74, 56);
      this.TrainingLibrary1.Name = "TrainingLibrary1";
      this.TrainingLibrary1.Size = new System.Drawing.Size (218, 24);
      this.TrainingLibrary1.TabIndex = 4;
      this.TrainingLibrary1.TabStop = false;
      this.TrainingLibrary1.SelectedIndexChanged += new System.EventHandler (this.TrainingLibrary1_SelectedIndexChanged);
      // 
      // TrainingLibrary2
      // 
      this.TrainingLibrary2.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TrainingLibrary2.FormattingEnabled = true;
      this.TrainingLibrary2.Location = new System.Drawing.Point (563, 54);
      this.TrainingLibrary2.Name = "TrainingLibrary2";
      this.TrainingLibrary2.Size = new System.Drawing.Size (218, 24);
      this.TrainingLibrary2.TabIndex = 6;
      this.TrainingLibrary2.TabStop = false;
      this.TrainingLibrary2.SelectedIndexChanged += new System.EventHandler (this.TrainingLibrary2_SelectedIndexChanged);
      // 
      // TrainingLibrary2Label
      // 
      this.TrainingLibrary2Label.AutoSize = true;
      this.TrainingLibrary2Label.Location = new System.Drawing.Point (496, 59);
      this.TrainingLibrary2Label.Name = "TrainingLibrary2Label";
      this.TrainingLibrary2Label.Size = new System.Drawing.Size (65, 13);
      this.TrainingLibrary2Label.TabIndex = 5;
      this.TrainingLibrary2Label.Text = "Train Library";
      // 
      // Lib1Pred1Label
      // 
      this.Lib1Pred1Label.AutoSize = true;
      this.Lib1Pred1Label.Location = new System.Drawing.Point (7, 91);
      this.Lib1Pred1Label.Name = "Lib1Pred1Label";
      this.Lib1Pred1Label.Size = new System.Drawing.Size (63, 13);
      this.Lib1Pred1Label.TabIndex = 7;
      this.Lib1Pred1Label.Text = "Prediction 1";
      // 
      // Lib1Pred1Class
      // 
      this.Lib1Pred1Class.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib1Pred1Class.Location = new System.Drawing.Point (74, 88);
      this.Lib1Pred1Class.Name = "Lib1Pred1Class";
      this.Lib1Pred1Class.ReadOnly = true;
      this.Lib1Pred1Class.Size = new System.Drawing.Size (255, 22);
      this.Lib1Pred1Class.TabIndex = 8;
      this.Lib1Pred1Class.TabStop = false;
      // 
      // Lib1Pred1Prob
      // 
      this.Lib1Pred1Prob.Font = new System.Drawing.Font ("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib1Pred1Prob.Location = new System.Drawing.Point (334, 88);
      this.Lib1Pred1Prob.Name = "Lib1Pred1Prob";
      this.Lib1Pred1Prob.ReadOnly = true;
      this.Lib1Pred1Prob.Size = new System.Drawing.Size (60, 22);
      this.Lib1Pred1Prob.TabIndex = 9;
      this.Lib1Pred1Prob.TabStop = false;
      this.Lib1Pred1Prob.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // Lib1Pred2Prob
      // 
      this.Lib1Pred2Prob.Font = new System.Drawing.Font ("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib1Pred2Prob.Location = new System.Drawing.Point (334, 114);
      this.Lib1Pred2Prob.Name = "Lib1Pred2Prob";
      this.Lib1Pred2Prob.ReadOnly = true;
      this.Lib1Pred2Prob.Size = new System.Drawing.Size (60, 22);
      this.Lib1Pred2Prob.TabIndex = 12;
      this.Lib1Pred2Prob.TabStop = false;
      this.Lib1Pred2Prob.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // Lib1Pred2Class
      // 
      this.Lib1Pred2Class.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib1Pred2Class.Location = new System.Drawing.Point (74, 114);
      this.Lib1Pred2Class.Name = "Lib1Pred2Class";
      this.Lib1Pred2Class.ReadOnly = true;
      this.Lib1Pred2Class.Size = new System.Drawing.Size (255, 22);
      this.Lib1Pred2Class.TabIndex = 11;
      this.Lib1Pred2Class.TabStop = false;
      // 
      // Lib1Pred2Label
      // 
      this.Lib1Pred2Label.AutoSize = true;
      this.Lib1Pred2Label.Location = new System.Drawing.Point (7, 117);
      this.Lib1Pred2Label.Name = "Lib1Pred2Label";
      this.Lib1Pred2Label.Size = new System.Drawing.Size (63, 13);
      this.Lib1Pred2Label.TabIndex = 10;
      this.Lib1Pred2Label.Text = "Prediction 2";
      // 
      // Lib2Pred2Prob
      // 
      this.Lib2Pred2Prob.Font = new System.Drawing.Font ("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib2Pred2Prob.Location = new System.Drawing.Point (823, 114);
      this.Lib2Pred2Prob.Name = "Lib2Pred2Prob";
      this.Lib2Pred2Prob.ReadOnly = true;
      this.Lib2Pred2Prob.Size = new System.Drawing.Size (60, 22);
      this.Lib2Pred2Prob.TabIndex = 18;
      this.Lib2Pred2Prob.TabStop = false;
      this.Lib2Pred2Prob.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // Lib2Pred2Class
      // 
      this.Lib2Pred2Class.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib2Pred2Class.Location = new System.Drawing.Point (563, 114);
      this.Lib2Pred2Class.Name = "Lib2Pred2Class";
      this.Lib2Pred2Class.ReadOnly = true;
      this.Lib2Pred2Class.Size = new System.Drawing.Size (255, 22);
      this.Lib2Pred2Class.TabIndex = 17;
      this.Lib2Pred2Class.TabStop = false;
      // 
      // Lib2Pred2Label
      // 
      this.Lib2Pred2Label.AutoSize = true;
      this.Lib2Pred2Label.Location = new System.Drawing.Point (496, 117);
      this.Lib2Pred2Label.Name = "Lib2Pred2Label";
      this.Lib2Pred2Label.Size = new System.Drawing.Size (63, 13);
      this.Lib2Pred2Label.TabIndex = 16;
      this.Lib2Pred2Label.Text = "Prediction 2";
      // 
      // Lib2Pred1Prob
      // 
      this.Lib2Pred1Prob.Font = new System.Drawing.Font ("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib2Pred1Prob.Location = new System.Drawing.Point (823, 88);
      this.Lib2Pred1Prob.Name = "Lib2Pred1Prob";
      this.Lib2Pred1Prob.ReadOnly = true;
      this.Lib2Pred1Prob.Size = new System.Drawing.Size (60, 22);
      this.Lib2Pred1Prob.TabIndex = 15;
      this.Lib2Pred1Prob.TabStop = false;
      this.Lib2Pred1Prob.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // Lib2Pred1Class
      // 
      this.Lib2Pred1Class.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib2Pred1Class.Location = new System.Drawing.Point (563, 88);
      this.Lib2Pred1Class.Name = "Lib2Pred1Class";
      this.Lib2Pred1Class.ReadOnly = true;
      this.Lib2Pred1Class.Size = new System.Drawing.Size (255, 22);
      this.Lib2Pred1Class.TabIndex = 14;
      this.Lib2Pred1Class.TabStop = false;
      // 
      // Lib2Pred1Label
      // 
      this.Lib2Pred1Label.AutoSize = true;
      this.Lib2Pred1Label.Location = new System.Drawing.Point (496, 91);
      this.Lib2Pred1Label.Name = "Lib2Pred1Label";
      this.Lib2Pred1Label.Size = new System.Drawing.Size (63, 13);
      this.Lib2Pred1Label.TabIndex = 13;
      this.Lib2Pred1Label.Text = "Prediction 1";
      // 
      // ZoomLabel
      // 
      this.ZoomLabel.AutoSize = true;
      this.ZoomLabel.Location = new System.Drawing.Point (7, 9);
      this.ZoomLabel.Name = "ZoomLabel";
      this.ZoomLabel.Size = new System.Drawing.Size (34, 13);
      this.ZoomLabel.TabIndex = 20;
      this.ZoomLabel.Text = "Zoom";
      // 
      // ZoomFactor
      // 
      this.ZoomFactor.Font = new System.Drawing.Font ("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ZoomFactor.FormattingEnabled = true;
      this.ZoomFactor.Items.AddRange (new object[] {
            "10%",
            "25%",
            "50%",
            "100%",
            "200%",
            "400%"});
      this.ZoomFactor.Location = new System.Drawing.Point (74, 6);
      this.ZoomFactor.Name = "ZoomFactor";
      this.ZoomFactor.Size = new System.Drawing.Size (84, 24);
      this.ZoomFactor.TabIndex = 21;
      this.ZoomFactor.Text = "100%";
      this.ZoomFactor.SelectedIndexChanged += new System.EventHandler (this.ZoomFactor_SelectedIndexChanged);
      this.ZoomFactor.Leave += new System.EventHandler (this.ZoomFactor_Leave);
      // 
      // RightPanel
      // 
      this.RightPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.RightPanel.Controls.Add (this.Lib2Pred2Votes);
      this.RightPanel.Controls.Add (this.Lib2Pred1Votes);
      this.RightPanel.Controls.Add (this.Lib1Pred2Votes);
      this.RightPanel.Controls.Add (this.Lib1Pred1Votes);
      this.RightPanel.Controls.Add (this.DisplayGridButton);
      this.RightPanel.Controls.Add (this.BreakDownTrainingLibrary2Button);
      this.RightPanel.Controls.Add (this.BreakDownTrainingLibrary1Button);
      this.RightPanel.Controls.Add (this.ExampleFileName);
      this.RightPanel.Controls.Add (this.RebuildTrainLibrary2Button);
      this.RightPanel.Controls.Add (this.RebuildTrainLibrary1Button);
      this.RightPanel.Controls.Add (this.ZoomFactor);
      this.RightPanel.Controls.Add (this.ZoomLabel);
      this.RightPanel.Controls.Add (this.TrainingLibrary1Label);
      this.RightPanel.Controls.Add (this.TrainingLibrary1);
      this.RightPanel.Controls.Add (this.Lib2Pred2Prob);
      this.RightPanel.Controls.Add (this.TrainingLibrary2Label);
      this.RightPanel.Controls.Add (this.Lib2Pred2Class);
      this.RightPanel.Controls.Add (this.TrainingLibrary2);
      this.RightPanel.Controls.Add (this.Lib2Pred2Label);
      this.RightPanel.Controls.Add (this.Lib1Pred1Label);
      this.RightPanel.Controls.Add (this.Lib2Pred1Prob);
      this.RightPanel.Controls.Add (this.Lib1Pred1Class);
      this.RightPanel.Controls.Add (this.Lib2Pred1Class);
      this.RightPanel.Controls.Add (this.Lib1Pred1Prob);
      this.RightPanel.Controls.Add (this.Lib2Pred1Label);
      this.RightPanel.Controls.Add (this.Lib1Pred2Label);
      this.RightPanel.Controls.Add (this.Lib1Pred2Prob);
      this.RightPanel.Controls.Add (this.Lib1Pred2Class);
      this.RightPanel.Location = new System.Drawing.Point (16, 12);
      this.RightPanel.Name = "RightPanel";
      this.RightPanel.Size = new System.Drawing.Size (975, 150);
      this.RightPanel.TabIndex = 22;
      this.RightPanel.SizeChanged += new System.EventHandler (this.RightPanel_SizeChanged);
      // 
      // Lib2Pred2Votes
      // 
      this.Lib2Pred2Votes.Font = new System.Drawing.Font ("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib2Pred2Votes.Location = new System.Drawing.Point (888, 114);
      this.Lib2Pred2Votes.Name = "Lib2Pred2Votes";
      this.Lib2Pred2Votes.ReadOnly = true;
      this.Lib2Pred2Votes.Size = new System.Drawing.Size (42, 22);
      this.Lib2Pred2Votes.TabIndex = 33;
      this.Lib2Pred2Votes.TabStop = false;
      this.Lib2Pred2Votes.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // Lib2Pred1Votes
      // 
      this.Lib2Pred1Votes.Font = new System.Drawing.Font ("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib2Pred1Votes.Location = new System.Drawing.Point (888, 88);
      this.Lib2Pred1Votes.Name = "Lib2Pred1Votes";
      this.Lib2Pred1Votes.ReadOnly = true;
      this.Lib2Pred1Votes.Size = new System.Drawing.Size (42, 22);
      this.Lib2Pred1Votes.TabIndex = 32;
      this.Lib2Pred1Votes.TabStop = false;
      this.Lib2Pred1Votes.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // Lib1Pred2Votes
      // 
      this.Lib1Pred2Votes.Font = new System.Drawing.Font ("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib1Pred2Votes.Location = new System.Drawing.Point (399, 114);
      this.Lib1Pred2Votes.Name = "Lib1Pred2Votes";
      this.Lib1Pred2Votes.ReadOnly = true;
      this.Lib1Pred2Votes.Size = new System.Drawing.Size (42, 22);
      this.Lib1Pred2Votes.TabIndex = 31;
      this.Lib1Pred2Votes.TabStop = false;
      this.Lib1Pred2Votes.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // Lib1Pred1Votes
      // 
      this.Lib1Pred1Votes.Font = new System.Drawing.Font ("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Lib1Pred1Votes.Location = new System.Drawing.Point (399, 88);
      this.Lib1Pred1Votes.Name = "Lib1Pred1Votes";
      this.Lib1Pred1Votes.ReadOnly = true;
      this.Lib1Pred1Votes.Size = new System.Drawing.Size (42, 22);
      this.Lib1Pred1Votes.TabIndex = 30;
      this.Lib1Pred1Votes.TabStop = false;
      this.Lib1Pred1Votes.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // DisplayGridButton
      // 
      this.DisplayGridButton.Location = new System.Drawing.Point (6, 26);
      this.DisplayGridButton.Name = "DisplayGridButton";
      this.DisplayGridButton.Size = new System.Drawing.Size (35, 23);
      this.DisplayGridButton.TabIndex = 29;
      this.DisplayGridButton.Text = "Grid";
      this.DisplayGridButton.UseVisualStyleBackColor = true;
      this.DisplayGridButton.Click += new System.EventHandler (this.DisplayGridButton_Click);
      // 
      // BreakDownTrainingLibrary2Button
      // 
      this.BreakDownTrainingLibrary2Button.Location = new System.Drawing.Point (791, 54);
      this.BreakDownTrainingLibrary2Button.Name = "BreakDownTrainingLibrary2Button";
      this.BreakDownTrainingLibrary2Button.Size = new System.Drawing.Size (71, 23);
      this.BreakDownTrainingLibrary2Button.TabIndex = 28;
      this.BreakDownTrainingLibrary2Button.TabStop = false;
      this.BreakDownTrainingLibrary2Button.Text = "BreakDown";
      this.BreakDownTrainingLibrary2Button.UseVisualStyleBackColor = true;
      this.BreakDownTrainingLibrary2Button.Click += new System.EventHandler (this.BreakDownTrainingLibrary2Button_Click);
      // 
      // BreakDownTrainingLibrary1Button
      // 
      this.BreakDownTrainingLibrary1Button.Location = new System.Drawing.Point (302, 57);
      this.BreakDownTrainingLibrary1Button.Name = "BreakDownTrainingLibrary1Button";
      this.BreakDownTrainingLibrary1Button.Size = new System.Drawing.Size (71, 23);
      this.BreakDownTrainingLibrary1Button.TabIndex = 27;
      this.BreakDownTrainingLibrary1Button.TabStop = false;
      this.BreakDownTrainingLibrary1Button.Text = "BreakDown";
      this.BreakDownTrainingLibrary1Button.UseVisualStyleBackColor = true;
      this.BreakDownTrainingLibrary1Button.Click += new System.EventHandler (this.BreakDownTrainingLibrary1Button_Click);
      // 
      // ExampleFileName
      // 
      this.ExampleFileName.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ExampleFileName.Location = new System.Drawing.Point (164, 6);
      this.ExampleFileName.Name = "ExampleFileName";
      this.ExampleFileName.ReadOnly = true;
      this.ExampleFileName.Size = new System.Drawing.Size (277, 22);
      this.ExampleFileName.TabIndex = 26;
      // 
      // RebuildTrainLibrary2Button
      // 
      this.RebuildTrainLibrary2Button.Location = new System.Drawing.Point (874, 54);
      this.RebuildTrainLibrary2Button.Name = "RebuildTrainLibrary2Button";
      this.RebuildTrainLibrary2Button.Size = new System.Drawing.Size (51, 23);
      this.RebuildTrainLibrary2Button.TabIndex = 25;
      this.RebuildTrainLibrary2Button.TabStop = false;
      this.RebuildTrainLibrary2Button.Text = "Rebuild";
      this.RebuildTrainLibrary2Button.UseVisualStyleBackColor = true;
      this.RebuildTrainLibrary2Button.Click += new System.EventHandler (this.RebuildTrainLibrary2Button_Click);
      // 
      // RebuildTrainLibrary1Button
      // 
      this.RebuildTrainLibrary1Button.Location = new System.Drawing.Point (385, 56);
      this.RebuildTrainLibrary1Button.Name = "RebuildTrainLibrary1Button";
      this.RebuildTrainLibrary1Button.Size = new System.Drawing.Size (51, 23);
      this.RebuildTrainLibrary1Button.TabIndex = 24;
      this.RebuildTrainLibrary1Button.TabStop = false;
      this.RebuildTrainLibrary1Button.Text = "Rebuild";
      this.RebuildTrainLibrary1Button.UseVisualStyleBackColor = true;
      this.RebuildTrainLibrary1Button.Click += new System.EventHandler (this.RebuildTrainLibrary1Button_Click);
      // 
      // LeftPanel
      // 
      this.LeftPanel.AutoScroll = true;
      this.LeftPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.LeftPanel.Controls.Add (this.PlanktonImage);
      this.LeftPanel.Location = new System.Drawing.Point (12, 168);
      this.LeftPanel.Name = "LeftPanel";
      this.LeftPanel.Size = new System.Drawing.Size (979, 464);
      this.LeftPanel.TabIndex = 23;
      this.LeftPanel.SizeChanged += new System.EventHandler (this.LeftPanel_SizeChanged);
      // 
      // ImageViewer
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (1003, 634);
      this.Controls.Add (this.LeftPanel);
      this.Controls.Add (this.RightPanel);
      this.MinimumSize = new System.Drawing.Size (16, 672);
      this.Name = "ImageViewer";
      this.Text = "ImageViewer";
      this.Load += new System.EventHandler (this.ImageViewer_Load);
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler (this.ImageViewer_FormClosing);
      this.Resize += new System.EventHandler (this.ImageViewer_Resize);
      ((System.ComponentModel.ISupportInitialize)(this.PlanktonImage)).EndInit ();
      this.RightPanel.ResumeLayout (false);
      this.RightPanel.PerformLayout ();
      this.LeftPanel.ResumeLayout (false);
      this.ResumeLayout (false);

    }

    #endregion

    private System.Windows.Forms.PictureBox PlanktonImage;
    private System.Windows.Forms.Label TrainingLibrary1Label;
    private System.Windows.Forms.ComboBox TrainingLibrary1;
    private System.Windows.Forms.ComboBox TrainingLibrary2;
    private System.Windows.Forms.Label TrainingLibrary2Label;
    private System.Windows.Forms.Label Lib1Pred1Label;
    private System.Windows.Forms.TextBox Lib1Pred1Class;
    private System.Windows.Forms.TextBox Lib1Pred1Prob;
    private System.Windows.Forms.TextBox Lib1Pred2Prob;
    private System.Windows.Forms.TextBox Lib1Pred2Class;
    private System.Windows.Forms.Label Lib1Pred2Label;
    private System.Windows.Forms.TextBox Lib2Pred2Prob;
    private System.Windows.Forms.TextBox Lib2Pred2Class;
    private System.Windows.Forms.Label Lib2Pred2Label;
    private System.Windows.Forms.TextBox Lib2Pred1Prob;
    private System.Windows.Forms.TextBox Lib2Pred1Class;
    private System.Windows.Forms.Label Lib2Pred1Label;
    private System.Windows.Forms.Label ZoomLabel;
    private System.Windows.Forms.ComboBox ZoomFactor;
    private System.Windows.Forms.Panel RightPanel;
    private System.Windows.Forms.Panel LeftPanel;
    private System.Windows.Forms.Button RebuildTrainLibrary2Button;
    private System.Windows.Forms.Button RebuildTrainLibrary1Button;
    private System.Windows.Forms.TextBox ExampleFileName;
    private System.Windows.Forms.Button BreakDownTrainingLibrary2Button;
    private System.Windows.Forms.Button BreakDownTrainingLibrary1Button;
    private System.Windows.Forms.Button DisplayGridButton;
    private System.Windows.Forms.TextBox Lib1Pred1Votes;
    private System.Windows.Forms.TextBox Lib2Pred2Votes;
    private System.Windows.Forms.TextBox Lib2Pred1Votes;
    private System.Windows.Forms.TextBox Lib1Pred2Votes;
  }
}