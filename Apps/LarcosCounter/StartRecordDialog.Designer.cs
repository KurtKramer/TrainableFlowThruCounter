namespace LarcosCounter
{
  partial class StartRecordDialog
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose (bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose ();
      }
      base.Dispose (disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent ()
    {
      this.DescriptionLabel = new System.Windows.Forms.Label ();
      this.Description = new System.Windows.Forms.TextBox ();
      this.RootNameLabel = new System.Windows.Forms.Label ();
      this.RootName = new System.Windows.Forms.TextBox ();
      this.ReBuildModel = new System.Windows.Forms.Button ();
      this.ClassifierLabel = new System.Windows.Forms.Label ();
      this.Classifier = new System.Windows.Forms.ComboBox ();
      this.SubjectLabel = new System.Windows.Forms.Label ();
      this.Subject = new System.Windows.Forms.ComboBox ();
      this.WaterQuality = new System.Windows.Forms.ComboBox ();
      this.WaterQualityLabel = new System.Windows.Forms.Label ();
      this.SuspendLayout ();
      // 
      // DescriptionLabel
      // 
      this.DescriptionLabel.AutoSize = true;
      this.DescriptionLabel.Location = new System.Drawing.Point (164, 16);
      this.DescriptionLabel.Name = "DescriptionLabel";
      this.DescriptionLabel.Size = new System.Drawing.Size (60, 13);
      this.DescriptionLabel.TabIndex = 227;
      this.DescriptionLabel.Text = "Description";
      // 
      // Description
      // 
      this.Description.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Description.Location = new System.Drawing.Point (167, 32);
      this.Description.Multiline = true;
      this.Description.Name = "Description";
      this.Description.Size = new System.Drawing.Size (440, 54);
      this.Description.TabIndex = 226;
      // 
      // RootNameLabel
      // 
      this.RootNameLabel.AutoSize = true;
      this.RootNameLabel.Location = new System.Drawing.Point (12, 16);
      this.RootNameLabel.Name = "RootNameLabel";
      this.RootNameLabel.Size = new System.Drawing.Size (63, 13);
      this.RootNameLabel.TabIndex = 225;
      this.RootNameLabel.Text = "Short Name";
      // 
      // RootName
      // 
      this.RootName.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.RootName.Location = new System.Drawing.Point (12, 32);
      this.RootName.Name = "RootName";
      this.RootName.Size = new System.Drawing.Size (134, 22);
      this.RootName.TabIndex = 224;
      // 
      // ReBuildModel
      // 
      this.ReBuildModel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ReBuildModel.Location = new System.Drawing.Point (335, 132);
      this.ReBuildModel.Name = "ReBuildModel";
      this.ReBuildModel.Size = new System.Drawing.Size (75, 23);
      this.ReBuildModel.TabIndex = 223;
      this.ReBuildModel.Text = "Rebuild Model";
      this.ReBuildModel.UseVisualStyleBackColor = true;
      // 
      // ClassifierLabel
      // 
      this.ClassifierLabel.AutoSize = true;
      this.ClassifierLabel.Location = new System.Drawing.Point (12, 218);
      this.ClassifierLabel.Name = "ClassifierLabel";
      this.ClassifierLabel.Size = new System.Drawing.Size (48, 13);
      this.ClassifierLabel.TabIndex = 221;
      this.ClassifierLabel.Text = "Classifier";
      this.ClassifierLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // Classifier
      // 
      this.Classifier.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Classifier.FormattingEnabled = true;
      this.Classifier.Location = new System.Drawing.Point (15, 231);
      this.Classifier.Name = "Classifier";
      this.Classifier.Size = new System.Drawing.Size (300, 24);
      this.Classifier.TabIndex = 222;
      this.Classifier.Text = "None";
      // 
      // SubjectLabel
      // 
      this.SubjectLabel.AutoSize = true;
      this.SubjectLabel.Location = new System.Drawing.Point (12, 116);
      this.SubjectLabel.Name = "SubjectLabel";
      this.SubjectLabel.Size = new System.Drawing.Size (43, 13);
      this.SubjectLabel.TabIndex = 228;
      this.SubjectLabel.Text = "Subject";
      this.SubjectLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // Subject
      // 
      this.Subject.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Subject.FormattingEnabled = true;
      this.Subject.Location = new System.Drawing.Point (12, 132);
      this.Subject.Name = "Subject";
      this.Subject.Size = new System.Drawing.Size (185, 24);
      this.Subject.TabIndex = 229;
      this.Subject.Text = "None";
      // 
      // WaterQuality
      // 
      this.WaterQuality.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.WaterQuality.FormattingEnabled = true;
      this.WaterQuality.Location = new System.Drawing.Point (223, 132);
      this.WaterQuality.Name = "WaterQuality";
      this.WaterQuality.Size = new System.Drawing.Size (92, 24);
      this.WaterQuality.TabIndex = 230;
      this.WaterQuality.Text = "None";
      // 
      // WaterQualityLabel
      // 
      this.WaterQualityLabel.AutoSize = true;
      this.WaterQualityLabel.Location = new System.Drawing.Point (223, 116);
      this.WaterQualityLabel.Name = "WaterQualityLabel";
      this.WaterQualityLabel.Size = new System.Drawing.Size (42, 13);
      this.WaterQualityLabel.TabIndex = 231;
      this.WaterQualityLabel.Text = "Qual;ity";
      this.WaterQualityLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // StartRecordDialog
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (633, 263);
      this.Controls.Add (this.WaterQualityLabel);
      this.Controls.Add (this.WaterQuality);
      this.Controls.Add (this.Subject);
      this.Controls.Add (this.SubjectLabel);
      this.Controls.Add (this.DescriptionLabel);
      this.Controls.Add (this.Description);
      this.Controls.Add (this.RootNameLabel);
      this.Controls.Add (this.RootName);
      this.Controls.Add (this.ReBuildModel);
      this.Controls.Add (this.ClassifierLabel);
      this.Controls.Add (this.Classifier);
      this.Name = "StartRecordDialog";
      this.Text = "StartRecordDialog";
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.Label DescriptionLabel;
    private System.Windows.Forms.TextBox Description;
    private System.Windows.Forms.Label RootNameLabel;
    private System.Windows.Forms.TextBox RootName;
    private System.Windows.Forms.Button ReBuildModel;
    private System.Windows.Forms.Label ClassifierLabel;
    private System.Windows.Forms.ComboBox Classifier;
    private System.Windows.Forms.Label SubjectLabel;
    private System.Windows.Forms.ComboBox Subject;
    private System.Windows.Forms.ComboBox WaterQuality;
    private System.Windows.Forms.Label WaterQualityLabel;
  }
}