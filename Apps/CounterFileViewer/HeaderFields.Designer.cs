namespace CounterFileViewer
{
  partial class HeaderFields
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
      this.FileNameLabel = new System.Windows.Forms.Label ();
      this.FileName = new System.Windows.Forms.TextBox ();
      this.HeaderFieldsDetails = new System.Windows.Forms.ListView ();
      this.FieldNameHeader = new System.Windows.Forms.ColumnHeader ();
      this.FieldValue = new System.Windows.Forms.ColumnHeader ();
      this.SuspendLayout ();
      // 
      // FileNameLabel
      // 
      this.FileNameLabel.AutoSize = true;
      this.FileNameLabel.Location = new System.Drawing.Point (13, 13);
      this.FileNameLabel.Name = "FileNameLabel";
      this.FileNameLabel.Size = new System.Drawing.Size (54, 13);
      this.FileNameLabel.TabIndex = 0;
      this.FileNameLabel.Text = "File Name";
      // 
      // FileName
      // 
      this.FileName.Location = new System.Drawing.Point (12, 29);
      this.FileName.Name = "FileName";
      this.FileName.ReadOnly = true;
      this.FileName.Size = new System.Drawing.Size (651, 20);
      this.FileName.TabIndex = 1;
      // 
      // HeaderFieldsDetails
      // 
      this.HeaderFieldsDetails.Columns.AddRange (new System.Windows.Forms.ColumnHeader[] {
            this.FieldNameHeader,
            this.FieldValue});
      this.HeaderFieldsDetails.Location = new System.Drawing.Point (12, 55);
      this.HeaderFieldsDetails.Name = "HeaderFieldsDetails";
      this.HeaderFieldsDetails.Size = new System.Drawing.Size (651, 482);
      this.HeaderFieldsDetails.TabIndex = 2;
      this.HeaderFieldsDetails.UseCompatibleStateImageBehavior = false;
      this.HeaderFieldsDetails.View = System.Windows.Forms.View.Details;
      // 
      // FieldNameHeader
      // 
      this.FieldNameHeader.Text = "FieldName";
      this.FieldNameHeader.Width = 140;
      // 
      // FieldValue
      // 
      this.FieldValue.Width = 430;
      // 
      // HeaderFields
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (675, 549);
      this.Controls.Add (this.HeaderFieldsDetails);
      this.Controls.Add (this.FileName);
      this.Controls.Add (this.FileNameLabel);
      this.Name = "HeaderFields";
      this.Text = "HeaderFields";
      this.Load += new System.EventHandler (this.HeaderFields_Load);
      this.SizeChanged += new System.EventHandler (this.HeaderFields_SizeChanged);
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.Label FileNameLabel;
    private System.Windows.Forms.TextBox FileName;
    private System.Windows.Forms.ListView HeaderFieldsDetails;
    private System.Windows.Forms.ColumnHeader FieldNameHeader;
    private System.Windows.Forms.ColumnHeader FieldValue;
  }
}