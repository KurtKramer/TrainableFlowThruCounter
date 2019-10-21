namespace CounterApplication
{
  /// <summary>
  /// Used to prompt users for parameters needed to peroform a recorduing a Playback session.
  /// </summary>
  partial class StartRecordingDialog
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
      this.components = new System.ComponentModel.Container ();
      this.sessionDescription = new System.Windows.Forms.TextBox ();
      this.SessionDescriptionLabel = new System.Windows.Forms.Label ();
      this.personInCharge = new System.Windows.Forms.TextBox ();
      this.PersonInChargeLabel = new System.Windows.Forms.Label ();
      this.comments = new System.Windows.Forms.TextBox ();
      this.CommentsLabel = new System.Windows.Forms.Label ();
      this.source = new System.Windows.Forms.TextBox ();
      this.SourceLabel = new System.Windows.Forms.Label ();
      this.DestinationLabel = new System.Windows.Forms.Label ();
      this.destination = new System.Windows.Forms.TextBox ();
      this.subject = new System.Windows.Forms.ComboBox ();
      this.SubjectLabel = new System.Windows.Forms.Label ();
      this.subjectSize = new System.Windows.Forms.ComboBox ();
      this.SizeLabel = new System.Windows.Forms.Label ();
      this.WaterQualityLabel = new System.Windows.Forms.Label ();
      this.waterQuality = new System.Windows.Forms.ComboBox ();
      this.StarButton = new System.Windows.Forms.Button ();
      this.CancelButton = new System.Windows.Forms.Button ();
      this.errorProvider1 = new System.Windows.Forms.ErrorProvider (this.components);
      this.controlNum = new System.Windows.Forms.TextBox ();
      this.ControlNumLabel = new System.Windows.Forms.Label ();
      this.AdvancedOptionsButton = new System.Windows.Forms.Button ();
      this.SetCameraParametersButton = new System.Windows.Forms.Button ();
      ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).BeginInit ();
      this.SuspendLayout ();
      // 
      // sessionDescription
      // 
      this.sessionDescription.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.sessionDescription.Location = new System.Drawing.Point (12, 95);
      this.sessionDescription.Name = "sessionDescription";
      this.sessionDescription.Size = new System.Drawing.Size (630, 26);
      this.sessionDescription.TabIndex = 200;
      // 
      // SessionDescriptionLabel
      // 
      this.SessionDescriptionLabel.AutoSize = true;
      this.SessionDescriptionLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SessionDescriptionLabel.Location = new System.Drawing.Point (15, 77);
      this.SessionDescriptionLabel.Name = "SessionDescriptionLabel";
      this.SessionDescriptionLabel.Size = new System.Drawing.Size (128, 16);
      this.SessionDescriptionLabel.TabIndex = 1;
      this.SessionDescriptionLabel.Text = "Session Description";
      // 
      // personInCharge
      // 
      this.personInCharge.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.personInCharge.Location = new System.Drawing.Point (12, 149);
      this.personInCharge.Name = "personInCharge";
      this.personInCharge.Size = new System.Drawing.Size (246, 26);
      this.personInCharge.TabIndex = 210;
      // 
      // PersonInChargeLabel
      // 
      this.PersonInChargeLabel.AutoSize = true;
      this.PersonInChargeLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.PersonInChargeLabel.Location = new System.Drawing.Point (15, 131);
      this.PersonInChargeLabel.Name = "PersonInChargeLabel";
      this.PersonInChargeLabel.Size = new System.Drawing.Size (111, 16);
      this.PersonInChargeLabel.TabIndex = 3;
      this.PersonInChargeLabel.Text = "Person in Charge";
      // 
      // comments
      // 
      this.comments.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.comments.Location = new System.Drawing.Point (12, 208);
      this.comments.Multiline = true;
      this.comments.Name = "comments";
      this.comments.Size = new System.Drawing.Size (630, 93);
      this.comments.TabIndex = 300;
      // 
      // CommentsLabel
      // 
      this.CommentsLabel.AutoSize = true;
      this.CommentsLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CommentsLabel.Location = new System.Drawing.Point (15, 190);
      this.CommentsLabel.Name = "CommentsLabel";
      this.CommentsLabel.Size = new System.Drawing.Size (72, 16);
      this.CommentsLabel.TabIndex = 5;
      this.CommentsLabel.Text = "Comments";
      // 
      // source
      // 
      this.source.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.source.Location = new System.Drawing.Point (322, 149);
      this.source.Name = "source";
      this.source.Size = new System.Drawing.Size (150, 26);
      this.source.TabIndex = 220;
      // 
      // SourceLabel
      // 
      this.SourceLabel.AutoSize = true;
      this.SourceLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SourceLabel.Location = new System.Drawing.Point (325, 131);
      this.SourceLabel.Name = "SourceLabel";
      this.SourceLabel.Size = new System.Drawing.Size (87, 16);
      this.SourceLabel.TabIndex = 7;
      this.SourceLabel.Text = "Source (tank)";
      // 
      // DestinationLabel
      // 
      this.DestinationLabel.AutoSize = true;
      this.DestinationLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.DestinationLabel.Location = new System.Drawing.Point (495, 131);
      this.DestinationLabel.Name = "DestinationLabel";
      this.DestinationLabel.Size = new System.Drawing.Size (111, 16);
      this.DestinationLabel.TabIndex = 9;
      this.DestinationLabel.Text = "Destination (tank)";
      // 
      // destination
      // 
      this.destination.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.destination.Location = new System.Drawing.Point (492, 149);
      this.destination.Name = "destination";
      this.destination.Size = new System.Drawing.Size (150, 26);
      this.destination.TabIndex = 230;
      // 
      // subject
      // 
      this.subject.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.subject.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.subject.FormattingEnabled = true;
      this.subject.Location = new System.Drawing.Point (12, 34);
      this.subject.Name = "subject";
      this.subject.Size = new System.Drawing.Size (170, 28);
      this.subject.TabIndex = 100;
      this.subject.SelectedIndexChanged += new System.EventHandler (this.subject_SelectedIndexChanged);
      // 
      // SubjectLabel
      // 
      this.SubjectLabel.AutoSize = true;
      this.SubjectLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SubjectLabel.Location = new System.Drawing.Point (15, 16);
      this.SubjectLabel.Name = "SubjectLabel";
      this.SubjectLabel.Size = new System.Drawing.Size (53, 16);
      this.SubjectLabel.TabIndex = 11;
      this.SubjectLabel.Text = "Subject";
      // 
      // subjectSize
      // 
      this.subjectSize.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.subjectSize.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.subjectSize.FormattingEnabled = true;
      this.subjectSize.Location = new System.Drawing.Point (197, 34);
      this.subjectSize.Name = "subjectSize";
      this.subjectSize.Size = new System.Drawing.Size (120, 28);
      this.subjectSize.TabIndex = 110;
      this.subjectSize.SelectedIndexChanged += new System.EventHandler (this.size_SelectedIndexChanged);
      // 
      // SizeLabel
      // 
      this.SizeLabel.AutoSize = true;
      this.SizeLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SizeLabel.Location = new System.Drawing.Point (200, 16);
      this.SizeLabel.Name = "SizeLabel";
      this.SizeLabel.Size = new System.Drawing.Size (34, 16);
      this.SizeLabel.TabIndex = 13;
      this.SizeLabel.Text = "Size";
      // 
      // WaterQualityLabel
      // 
      this.WaterQualityLabel.AutoSize = true;
      this.WaterQualityLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.WaterQualityLabel.Location = new System.Drawing.Point (339, 16);
      this.WaterQualityLabel.Name = "WaterQualityLabel";
      this.WaterQualityLabel.Size = new System.Drawing.Size (88, 16);
      this.WaterQualityLabel.TabIndex = 15;
      this.WaterQualityLabel.Text = "Water Quality";
      // 
      // waterQuality
      // 
      this.waterQuality.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.waterQuality.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.waterQuality.FormattingEnabled = true;
      this.waterQuality.Location = new System.Drawing.Point (336, 34);
      this.waterQuality.Name = "waterQuality";
      this.waterQuality.Size = new System.Drawing.Size (160, 28);
      this.waterQuality.TabIndex = 120;
      this.waterQuality.SelectedIndexChanged += new System.EventHandler (this.waterQuality_SelectedIndexChanged);
      // 
      // StarButton
      // 
      this.StarButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.StarButton.Location = new System.Drawing.Point (12, 320);
      this.StarButton.Name = "StarButton";
      this.StarButton.Size = new System.Drawing.Size (75, 32);
      this.StarButton.TabIndex = 500;
      this.StarButton.Text = "Start";
      this.StarButton.UseVisualStyleBackColor = true;
      this.StarButton.Click += new System.EventHandler (this.StarButton_Click);
      // 
      // CancelButton
      // 
      this.CancelButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CancelButton.Location = new System.Drawing.Point (567, 320);
      this.CancelButton.Name = "CancelButton";
      this.CancelButton.Size = new System.Drawing.Size (75, 32);
      this.CancelButton.TabIndex = 530;
      this.CancelButton.Text = "Cancel";
      this.CancelButton.UseVisualStyleBackColor = true;
      this.CancelButton.Click += new System.EventHandler (this.CancelButton_Click);
      // 
      // errorProvider1
      // 
      this.errorProvider1.ContainerControl = this;
      // 
      // controlNum
      // 
      this.controlNum.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.controlNum.Location = new System.Drawing.Point (516, 34);
      this.controlNum.Name = "controlNum";
      this.controlNum.Size = new System.Drawing.Size (126, 26);
      this.controlNum.TabIndex = 130;
      // 
      // ControlNumLabel
      // 
      this.ControlNumLabel.AutoSize = true;
      this.ControlNumLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ControlNumLabel.Location = new System.Drawing.Point (519, 16);
      this.ControlNumLabel.Name = "ControlNumLabel";
      this.ControlNumLabel.Size = new System.Drawing.Size (81, 16);
      this.ControlNumLabel.TabIndex = 19;
      this.ControlNumLabel.Text = "Control Num";
      // 
      // AdvancedOptionsButton
      // 
      this.AdvancedOptionsButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AdvancedOptionsButton.Location = new System.Drawing.Point (131, 320);
      this.AdvancedOptionsButton.Name = "AdvancedOptionsButton";
      this.AdvancedOptionsButton.Size = new System.Drawing.Size (171, 32);
      this.AdvancedOptionsButton.TabIndex = 510;
      this.AdvancedOptionsButton.Text = "Advanced Options";
      this.AdvancedOptionsButton.UseVisualStyleBackColor = true;
      this.AdvancedOptionsButton.Click += new System.EventHandler (this.AdvancedOptionsButton_Click);
      // 
      // SetCameraParametersButton
      // 
      this.SetCameraParametersButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SetCameraParametersButton.Location = new System.Drawing.Point (352, 320);
      this.SetCameraParametersButton.Name = "SetCameraParametersButton";
      this.SetCameraParametersButton.Size = new System.Drawing.Size (171, 32);
      this.SetCameraParametersButton.TabIndex = 520;
      this.SetCameraParametersButton.Text = "Set Camera Params";
      this.SetCameraParametersButton.UseVisualStyleBackColor = true;
      this.SetCameraParametersButton.Click += new System.EventHandler (this.SetCameraParametersButton_Click);
      // 
      // StartRecordingDialog
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (657, 366);
      this.Controls.Add (this.SetCameraParametersButton);
      this.Controls.Add (this.AdvancedOptionsButton);
      this.Controls.Add (this.ControlNumLabel);
      this.Controls.Add (this.controlNum);
      this.Controls.Add (this.CancelButton);
      this.Controls.Add (this.StarButton);
      this.Controls.Add (this.WaterQualityLabel);
      this.Controls.Add (this.waterQuality);
      this.Controls.Add (this.SizeLabel);
      this.Controls.Add (this.subjectSize);
      this.Controls.Add (this.SubjectLabel);
      this.Controls.Add (this.subject);
      this.Controls.Add (this.DestinationLabel);
      this.Controls.Add (this.destination);
      this.Controls.Add (this.SourceLabel);
      this.Controls.Add (this.source);
      this.Controls.Add (this.CommentsLabel);
      this.Controls.Add (this.comments);
      this.Controls.Add (this.PersonInChargeLabel);
      this.Controls.Add (this.personInCharge);
      this.Controls.Add (this.SessionDescriptionLabel);
      this.Controls.Add (this.sessionDescription);
      this.MinimumSize = new System.Drawing.Size (673, 404);
      this.Name = "StartRecordingDialog";
      this.Text = "Start Counting Session";
      this.Load += new System.EventHandler (this.StartRecordingDialog_Load);
      ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).EndInit ();
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.TextBox sessionDescription;
    private System.Windows.Forms.Label SessionDescriptionLabel;
    private System.Windows.Forms.TextBox personInCharge;
    private System.Windows.Forms.Label PersonInChargeLabel;
    private System.Windows.Forms.TextBox comments;
    private System.Windows.Forms.Label CommentsLabel;
    private System.Windows.Forms.TextBox source;
    private System.Windows.Forms.Label SourceLabel;
    private System.Windows.Forms.Label DestinationLabel;
    private System.Windows.Forms.TextBox destination;
    private System.Windows.Forms.ComboBox subject;
    private System.Windows.Forms.Label SubjectLabel;
    private System.Windows.Forms.ComboBox subjectSize;
    private System.Windows.Forms.Label SizeLabel;
    private System.Windows.Forms.Label WaterQualityLabel;
    private System.Windows.Forms.ComboBox waterQuality;
    private System.Windows.Forms.Button StarButton;
    private System.Windows.Forms.Button CancelButton;
    private System.Windows.Forms.ErrorProvider errorProvider1;
    private System.Windows.Forms.TextBox controlNum;
    private System.Windows.Forms.Label ControlNumLabel;
    private System.Windows.Forms.Button AdvancedOptionsButton;
    private System.Windows.Forms.Button SetCameraParametersButton;
  }
}