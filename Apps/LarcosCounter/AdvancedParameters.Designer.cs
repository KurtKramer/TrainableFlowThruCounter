namespace LarcosCounter
{
  partial class AdvancedParameters
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
      this.FlatFieldCorrectionEnabledField = new System.Windows.Forms.CheckBox ();
      this.FileFormatField = new System.Windows.Forms.ComboBox ();
      this.DataIsToBeRecordedField = new System.Windows.Forms.CheckBox ();
      this.FileFormatLabel = new System.Windows.Forms.Label ();
      this.SaveParticleImagesField = new System.Windows.Forms.CheckBox ();
      this.DataIsToBeCountedField = new System.Windows.Forms.CheckBox ();
      this.OkButton = new System.Windows.Forms.Button ();
      this.CancelButton = new System.Windows.Forms.Button ();
      this.RequestedScanRateLabel = new System.Windows.Forms.Label ();
      this.RequestedScanRateField = new System.Windows.Forms.NumericUpDown ();
      this.BackGroundPixelTHLabel = new System.Windows.Forms.Label ();
      this.BackGroundPixelTHField = new System.Windows.Forms.NumericUpDown ();
      this.ErosionStructSizeField = new System.Windows.Forms.NumericUpDown ();
      this.StructSizeLabel = new System.Windows.Forms.Label ();
      this.MinSizeThresholdField = new System.Windows.Forms.TextBox ();
      this.MinSizeThresholdLabel = new System.Windows.Forms.Label ();
      this.FlowRatelabel = new System.Windows.Forms.Label ();
      this.flowRateFactorField = new System.Windows.Forms.NumericUpDown ();
      ((System.ComponentModel.ISupportInitialize)(this.RequestedScanRateField)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.BackGroundPixelTHField)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.ErosionStructSizeField)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.flowRateFactorField)).BeginInit ();
      this.SuspendLayout ();
      // 
      // FlatFieldCorrectionEnabledField
      // 
      this.FlatFieldCorrectionEnabledField.AutoSize = true;
      this.FlatFieldCorrectionEnabledField.Checked = true;
      this.FlatFieldCorrectionEnabledField.CheckState = System.Windows.Forms.CheckState.Checked;
      this.FlatFieldCorrectionEnabledField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.FlatFieldCorrectionEnabledField.Location = new System.Drawing.Point (12, 150);
      this.FlatFieldCorrectionEnabledField.Name = "FlatFieldCorrectionEnabledField";
      this.FlatFieldCorrectionEnabledField.Size = new System.Drawing.Size (200, 20);
      this.FlatFieldCorrectionEnabledField.TabIndex = 140;
      this.FlatFieldCorrectionEnabledField.Text = "Flat Field Correction Enabled";
      this.FlatFieldCorrectionEnabledField.UseVisualStyleBackColor = true;
      // 
      // FileFormatField
      // 
      this.FileFormatField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.FileFormatField.FormattingEnabled = true;
      this.FileFormatField.Location = new System.Drawing.Point (92, 194);
      this.FileFormatField.Name = "FileFormatField";
      this.FileFormatField.Size = new System.Drawing.Size (162, 28);
      this.FileFormatField.TabIndex = 150;
      this.FileFormatField.Text = "3BitEncoded";
      // 
      // DataIsToBeRecordedField
      // 
      this.DataIsToBeRecordedField.AutoSize = true;
      this.DataIsToBeRecordedField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.DataIsToBeRecordedField.Location = new System.Drawing.Point (12, 110);
      this.DataIsToBeRecordedField.Name = "DataIsToBeRecordedField";
      this.DataIsToBeRecordedField.Size = new System.Drawing.Size (122, 20);
      this.DataIsToBeRecordedField.TabIndex = 130;
      this.DataIsToBeRecordedField.Text = "Record To Disk";
      this.DataIsToBeRecordedField.UseVisualStyleBackColor = true;
      // 
      // FileFormatLabel
      // 
      this.FileFormatLabel.AutoSize = true;
      this.FileFormatLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.FileFormatLabel.Location = new System.Drawing.Point (10, 200);
      this.FileFormatLabel.Name = "FileFormatLabel";
      this.FileFormatLabel.Size = new System.Drawing.Size (75, 16);
      this.FileFormatLabel.TabIndex = 231;
      this.FileFormatLabel.Text = "File Format";
      this.FileFormatLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // SaveParticleImagesField
      // 
      this.SaveParticleImagesField.AutoSize = true;
      this.SaveParticleImagesField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SaveParticleImagesField.Location = new System.Drawing.Point (12, 70);
      this.SaveParticleImagesField.Name = "SaveParticleImagesField";
      this.SaveParticleImagesField.Size = new System.Drawing.Size (107, 20);
      this.SaveParticleImagesField.TabIndex = 120;
      this.SaveParticleImagesField.Text = "Save Images";
      this.SaveParticleImagesField.UseVisualStyleBackColor = true;
      // 
      // DataIsToBeCountedField
      // 
      this.DataIsToBeCountedField.AutoSize = true;
      this.DataIsToBeCountedField.Checked = true;
      this.DataIsToBeCountedField.CheckState = System.Windows.Forms.CheckState.Checked;
      this.DataIsToBeCountedField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.DataIsToBeCountedField.Location = new System.Drawing.Point (12, 30);
      this.DataIsToBeCountedField.Name = "DataIsToBeCountedField";
      this.DataIsToBeCountedField.Size = new System.Drawing.Size (116, 20);
      this.DataIsToBeCountedField.TabIndex = 110;
      this.DataIsToBeCountedField.Text = "Count Particles";
      this.DataIsToBeCountedField.UseVisualStyleBackColor = true;
      // 
      // OkButton
      // 
      this.OkButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.OkButton.Location = new System.Drawing.Point (12, 335);
      this.OkButton.Name = "OkButton";
      this.OkButton.Size = new System.Drawing.Size (76, 42);
      this.OkButton.TabIndex = 300;
      this.OkButton.Text = "OK";
      this.OkButton.UseVisualStyleBackColor = true;
      this.OkButton.Click += new System.EventHandler (this.OkButton_Click);
      // 
      // CancelButton
      // 
      this.CancelButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CancelButton.Location = new System.Drawing.Point (421, 335);
      this.CancelButton.Name = "CancelButton";
      this.CancelButton.Size = new System.Drawing.Size (75, 42);
      this.CancelButton.TabIndex = 310;
      this.CancelButton.Text = "Cancel";
      this.CancelButton.UseVisualStyleBackColor = true;
      this.CancelButton.Click += new System.EventHandler (this.CancelButton_Click);
      // 
      // RequestedScanRateLabel
      // 
      this.RequestedScanRateLabel.AutoSize = true;
      this.RequestedScanRateLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.RequestedScanRateLabel.Location = new System.Drawing.Point (9, 250);
      this.RequestedScanRateLabel.Name = "RequestedScanRateLabel";
      this.RequestedScanRateLabel.Size = new System.Drawing.Size (68, 16);
      this.RequestedScanRateLabel.TabIndex = 234;
      this.RequestedScanRateLabel.Text = "ScanRate";
      this.RequestedScanRateLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // RequestedScanRateField
      // 
      this.RequestedScanRateField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.RequestedScanRateField.Increment = new decimal (new int[] {
            100,
            0,
            0,
            0});
      this.RequestedScanRateField.Location = new System.Drawing.Point (92, 245);
      this.RequestedScanRateField.Maximum = new decimal (new int[] {
            35000,
            0,
            0,
            0});
      this.RequestedScanRateField.Minimum = new decimal (new int[] {
            1000,
            0,
            0,
            0});
      this.RequestedScanRateField.Name = "RequestedScanRateField";
      this.RequestedScanRateField.Size = new System.Drawing.Size (120, 26);
      this.RequestedScanRateField.TabIndex = 160;
      this.RequestedScanRateField.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.RequestedScanRateField.Value = new decimal (new int[] {
            1000,
            0,
            0,
            0});
      // 
      // BackGroundPixelTHLabel
      // 
      this.BackGroundPixelTHLabel.AutoSize = true;
      this.BackGroundPixelTHLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.BackGroundPixelTHLabel.Location = new System.Drawing.Point (350, 131);
      this.BackGroundPixelTHLabel.Name = "BackGroundPixelTHLabel";
      this.BackGroundPixelTHLabel.Size = new System.Drawing.Size (69, 16);
      this.BackGroundPixelTHLabel.TabIndex = 241;
      this.BackGroundPixelTHLabel.Text = "Threshold";
      this.BackGroundPixelTHLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // BackGroundPixelTHField
      // 
      this.BackGroundPixelTHField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.BackGroundPixelTHField.Location = new System.Drawing.Point (426, 127);
      this.BackGroundPixelTHField.Maximum = new decimal (new int[] {
            255,
            0,
            0,
            0});
      this.BackGroundPixelTHField.Name = "BackGroundPixelTHField";
      this.BackGroundPixelTHField.Size = new System.Drawing.Size (70, 26);
      this.BackGroundPixelTHField.TabIndex = 220;
      this.BackGroundPixelTHField.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.BackGroundPixelTHField.Value = new decimal (new int[] {
            31,
            0,
            0,
            0});
      // 
      // ErosionStructSizeField
      // 
      this.ErosionStructSizeField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ErosionStructSizeField.Location = new System.Drawing.Point (426, 25);
      this.ErosionStructSizeField.Maximum = new decimal (new int[] {
            8,
            0,
            0,
            0});
      this.ErosionStructSizeField.Name = "ErosionStructSizeField";
      this.ErosionStructSizeField.Size = new System.Drawing.Size (70, 26);
      this.ErosionStructSizeField.TabIndex = 200;
      this.ErosionStructSizeField.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.ErosionStructSizeField.Value = new decimal (new int[] {
            10,
            0,
            0,
            65536});
      // 
      // StructSizeLabel
      // 
      this.StructSizeLabel.AutoSize = true;
      this.StructSizeLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.StructSizeLabel.Location = new System.Drawing.Point (350, 31);
      this.StructSizeLabel.Name = "StructSizeLabel";
      this.StructSizeLabel.Size = new System.Drawing.Size (70, 16);
      this.StructSizeLabel.TabIndex = 239;
      this.StructSizeLabel.Text = "Struct Size";
      this.StructSizeLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // MinSizeThresholdField
      // 
      this.MinSizeThresholdField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.MinSizeThresholdField.Location = new System.Drawing.Point (426, 77);
      this.MinSizeThresholdField.Name = "MinSizeThresholdField";
      this.MinSizeThresholdField.Size = new System.Drawing.Size (70, 26);
      this.MinSizeThresholdField.TabIndex = 210;
      this.MinSizeThresholdField.Text = "5000";
      this.MinSizeThresholdField.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // MinSizeThresholdLabel
      // 
      this.MinSizeThresholdLabel.AutoSize = true;
      this.MinSizeThresholdLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.MinSizeThresholdLabel.Location = new System.Drawing.Point (350, 81);
      this.MinSizeThresholdLabel.Name = "MinSizeThresholdLabel";
      this.MinSizeThresholdLabel.Size = new System.Drawing.Size (58, 16);
      this.MinSizeThresholdLabel.TabIndex = 236;
      this.MinSizeThresholdLabel.Text = "Min Size";
      this.MinSizeThresholdLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // FlowRatelabel
      // 
      this.FlowRatelabel.AutoSize = true;
      this.FlowRatelabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.FlowRatelabel.Location = new System.Drawing.Point (350, 181);
      this.FlowRatelabel.Name = "FlowRatelabel";
      this.FlowRatelabel.Size = new System.Drawing.Size (68, 16);
      this.FlowRatelabel.TabIndex = 514;
      this.FlowRatelabel.Text = "Flow Rate";
      // 
      // flowRateFactorField
      // 
      this.flowRateFactorField.DecimalPlaces = 1;
      this.flowRateFactorField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.flowRateFactorField.Increment = new decimal (new int[] {
            1,
            0,
            0,
            65536});
      this.flowRateFactorField.Location = new System.Drawing.Point (426, 176);
      this.flowRateFactorField.Maximum = new decimal (new int[] {
            100,
            0,
            0,
            65536});
      this.flowRateFactorField.Minimum = new decimal (new int[] {
            1,
            0,
            0,
            65536});
      this.flowRateFactorField.Name = "flowRateFactorField";
      this.flowRateFactorField.Size = new System.Drawing.Size (70, 26);
      this.flowRateFactorField.TabIndex = 230;
      this.flowRateFactorField.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.flowRateFactorField.Value = new decimal (new int[] {
            10,
            0,
            0,
            65536});
      // 
      // AdvancedParameters
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (522, 394);
      this.Controls.Add (this.FlowRatelabel);
      this.Controls.Add (this.flowRateFactorField);
      this.Controls.Add (this.BackGroundPixelTHLabel);
      this.Controls.Add (this.BackGroundPixelTHField);
      this.Controls.Add (this.ErosionStructSizeField);
      this.Controls.Add (this.StructSizeLabel);
      this.Controls.Add (this.MinSizeThresholdField);
      this.Controls.Add (this.MinSizeThresholdLabel);
      this.Controls.Add (this.RequestedScanRateField);
      this.Controls.Add (this.RequestedScanRateLabel);
      this.Controls.Add (this.CancelButton);
      this.Controls.Add (this.OkButton);
      this.Controls.Add (this.FlatFieldCorrectionEnabledField);
      this.Controls.Add (this.DataIsToBeCountedField);
      this.Controls.Add (this.FileFormatField);
      this.Controls.Add (this.SaveParticleImagesField);
      this.Controls.Add (this.DataIsToBeRecordedField);
      this.Controls.Add (this.FileFormatLabel);
      this.MaximumSize = new System.Drawing.Size (538, 432);
      this.MinimumSize = new System.Drawing.Size (538, 432);
      this.Name = "AdvancedParameters";
      this.Text = "Advanced Parameters";
      this.Load += new System.EventHandler (this.AdvancedParameters_Load);
      ((System.ComponentModel.ISupportInitialize)(this.RequestedScanRateField)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.BackGroundPixelTHField)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.ErosionStructSizeField)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.flowRateFactorField)).EndInit ();
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.CheckBox FlatFieldCorrectionEnabledField;
    private System.Windows.Forms.ComboBox FileFormatField;
    private System.Windows.Forms.CheckBox DataIsToBeRecordedField;
    private System.Windows.Forms.Label FileFormatLabel;
    private System.Windows.Forms.CheckBox SaveParticleImagesField;
    private System.Windows.Forms.CheckBox DataIsToBeCountedField;
    private System.Windows.Forms.Button OkButton;
    private System.Windows.Forms.Button CancelButton;
    private System.Windows.Forms.Label RequestedScanRateLabel;
    private System.Windows.Forms.NumericUpDown RequestedScanRateField;
    private System.Windows.Forms.Label BackGroundPixelTHLabel;
    private System.Windows.Forms.NumericUpDown BackGroundPixelTHField;
    private System.Windows.Forms.NumericUpDown ErosionStructSizeField;
    private System.Windows.Forms.Label StructSizeLabel;
    private System.Windows.Forms.TextBox MinSizeThresholdField;
    private System.Windows.Forms.Label MinSizeThresholdLabel;
    private System.Windows.Forms.Label FlowRatelabel;
    private System.Windows.Forms.NumericUpDown flowRateFactorField;
  }
}