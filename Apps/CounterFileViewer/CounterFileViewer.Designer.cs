namespace CounterFileViewer
{
  partial class CounterFileViewer
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
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CounterFileViewer));
      this.ScannerPanel = new System.Windows.Forms.Panel();
      this.vScrollBar1 = new System.Windows.Forms.VScrollBar();
      this.FileName = new System.Windows.Forms.TextBox();
      this.FileNameLabel = new System.Windows.Forms.Label();
      this.menuStrip1 = new System.Windows.Forms.MenuStrip();
      this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.openScannerFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.autoDetectionToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
      this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.headerInfoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.displayHeaderFieldsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.cameraLineToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.StatusUpdateTimer = new System.Windows.Forms.Timer(this.components);
      this.TotalScanLinesLabel = new System.Windows.Forms.Label();
      this.SanLinesTotal = new System.Windows.Forms.TextBox();
      this.MouseCoordinates = new System.Windows.Forms.TextBox();
      this.MouseCoordinatesLabel = new System.Windows.Forms.Label();
      this.AutoScrollStop = new System.Windows.Forms.Button();
      this.AutoScrollingRate = new System.Windows.Forms.TextBox();
      this.AutoScrollDown = new System.Windows.Forms.Button();
      this.AutoScrollUp = new System.Windows.Forms.Button();
      this.autoDetect = new System.Windows.Forms.CheckBox();
      this.colorize = new System.Windows.Forms.CheckBox();
      this.cropColLeft = new System.Windows.Forms.TextBox();
      this.cropColRight = new System.Windows.Forms.TextBox();
      this.ScanLinesDisplayedLabel = new System.Windows.Forms.Label();
      this.ScanLinesDisplayed = new System.Windows.Forms.TextBox();
      this.AutoScrollingRateLabel = new System.Windows.Forms.Label();
      this.ScrollingPannel = new System.Windows.Forms.Panel();
      this.groundTruth = new System.Windows.Forms.CheckBox();
      this.menuStrip1.SuspendLayout();
      this.ScrollingPannel.SuspendLayout();
      this.SuspendLayout();
      // 
      // ScannerPanel
      // 
      this.ScannerPanel.BackColor = System.Drawing.SystemColors.Window;
      this.ScannerPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.ScannerPanel.Location = new System.Drawing.Point(15, 150);
      this.ScannerPanel.Name = "ScannerPanel";
      this.ScannerPanel.Size = new System.Drawing.Size(682, 537);
      this.ScannerPanel.TabIndex = 0;
      this.ScannerPanel.Paint += new System.Windows.Forms.PaintEventHandler(this.ScannerPanel_Paint);
      this.ScannerPanel.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.ScannerPanel_MouseDoubleClick);
      this.ScannerPanel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.ScannerPanel_MouseDown);
      this.ScannerPanel.MouseMove += new System.Windows.Forms.MouseEventHandler(this.ScannerPanel_MouseMove);
      this.ScannerPanel.MouseUp += new System.Windows.Forms.MouseEventHandler(this.ScannerPanel_MouseUp);
      // 
      // vScrollBar1
      // 
      this.vScrollBar1.Location = new System.Drawing.Point(697, 150);
      this.vScrollBar1.Margin = new System.Windows.Forms.Padding(1);
      this.vScrollBar1.Name = "vScrollBar1";
      this.vScrollBar1.Size = new System.Drawing.Size(22, 535);
      this.vScrollBar1.TabIndex = 1;
      this.vScrollBar1.Scroll += new System.Windows.Forms.ScrollEventHandler(this.vScrollBar1_Scroll);
      // 
      // FileName
      // 
      this.FileName.BorderStyle = System.Windows.Forms.BorderStyle.None;
      this.FileName.Enabled = false;
      this.FileName.Font = new System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.FileName.Location = new System.Drawing.Point(12, 52);
      this.FileName.Name = "FileName";
      this.FileName.ReadOnly = true;
      this.FileName.Size = new System.Drawing.Size(705, 15);
      this.FileName.TabIndex = 6;
      // 
      // FileNameLabel
      // 
      this.FileNameLabel.AutoSize = true;
      this.FileNameLabel.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.FileNameLabel.Location = new System.Drawing.Point(12, 35);
      this.FileNameLabel.Name = "FileNameLabel";
      this.FileNameLabel.Size = new System.Drawing.Size(56, 17);
      this.FileNameLabel.TabIndex = 8;
      this.FileNameLabel.Text = "File Name";
      this.FileNameLabel.UseCompatibleTextRendering = true;
      // 
      // menuStrip1
      // 
      this.menuStrip1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(225)))), ((int)(((byte)(225)))), ((int)(((byte)(255)))));
      this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.headerInfoToolStripMenuItem,
            this.helpToolStripMenuItem});
      this.menuStrip1.Location = new System.Drawing.Point(0, 0);
      this.menuStrip1.Name = "menuStrip1";
      this.menuStrip1.Size = new System.Drawing.Size(732, 24);
      this.menuStrip1.TabIndex = 9;
      this.menuStrip1.Text = "menuStrip1";
      // 
      // fileToolStripMenuItem
      // 
      this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openScannerFileToolStripMenuItem,
            this.autoDetectionToolStripMenuItem1,
            this.exitToolStripMenuItem});
      this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
      this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
      this.fileToolStripMenuItem.Text = "&File";
      // 
      // openScannerFileToolStripMenuItem
      // 
      this.openScannerFileToolStripMenuItem.Name = "openScannerFileToolStripMenuItem";
      this.openScannerFileToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
      this.openScannerFileToolStripMenuItem.Text = "&Open Scanner File";
      this.openScannerFileToolStripMenuItem.Click += new System.EventHandler(this.openScannerFileToolStripMenuItem_Click);
      // 
      // autoDetectionToolStripMenuItem1
      // 
      this.autoDetectionToolStripMenuItem1.Name = "autoDetectionToolStripMenuItem1";
      this.autoDetectionToolStripMenuItem1.Size = new System.Drawing.Size(169, 22);
      this.autoDetectionToolStripMenuItem1.Text = "Finale Report";
      this.autoDetectionToolStripMenuItem1.Click += new System.EventHandler(this.autoDetectionToolStripMenuItem1_Click);
      // 
      // exitToolStripMenuItem
      // 
      this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
      this.exitToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
      this.exitToolStripMenuItem.Text = "Exit";
      this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
      // 
      // headerInfoToolStripMenuItem
      // 
      this.headerInfoToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.displayHeaderFieldsToolStripMenuItem,
            this.cameraLineToolStripMenuItem});
      this.headerInfoToolStripMenuItem.Name = "headerInfoToolStripMenuItem";
      this.headerInfoToolStripMenuItem.Size = new System.Drawing.Size(81, 20);
      this.headerInfoToolStripMenuItem.Text = "Header Info";
      // 
      // displayHeaderFieldsToolStripMenuItem
      // 
      this.displayHeaderFieldsToolStripMenuItem.Name = "displayHeaderFieldsToolStripMenuItem";
      this.displayHeaderFieldsToolStripMenuItem.Size = new System.Drawing.Size(186, 22);
      this.displayHeaderFieldsToolStripMenuItem.Text = "Display Header Fields";
      this.displayHeaderFieldsToolStripMenuItem.Click += new System.EventHandler(this.headerInfoToolStripMenuItem_Click);
      // 
      // cameraLineToolStripMenuItem
      // 
      this.cameraLineToolStripMenuItem.Name = "cameraLineToolStripMenuItem";
      this.cameraLineToolStripMenuItem.Size = new System.Drawing.Size(186, 22);
      this.cameraLineToolStripMenuItem.Text = "Camera Line";
      this.cameraLineToolStripMenuItem.Click += new System.EventHandler(this.cameraLineToolStripMenuItem_Click);
      // 
      // helpToolStripMenuItem
      // 
      this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aboutToolStripMenuItem});
      this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
      this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
      this.helpToolStripMenuItem.Text = "&Help";
      // 
      // aboutToolStripMenuItem
      // 
      this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
      this.aboutToolStripMenuItem.Size = new System.Drawing.Size(107, 22);
      this.aboutToolStripMenuItem.Text = "About";
      // 
      // StatusUpdateTimer
      // 
      this.StatusUpdateTimer.Interval = 33;
      this.StatusUpdateTimer.Tick += new System.EventHandler(this.StatusUpdateTimer_Tick);
      // 
      // TotalScanLinesLabel
      // 
      this.TotalScanLinesLabel.AutoSize = true;
      this.TotalScanLinesLabel.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TotalScanLinesLabel.Location = new System.Drawing.Point(598, 28);
      this.TotalScanLinesLabel.Name = "TotalScanLinesLabel";
      this.TotalScanLinesLabel.Size = new System.Drawing.Size(26, 17);
      this.TotalScanLinesLabel.TabIndex = 14;
      this.TotalScanLinesLabel.Text = "Size";
      this.TotalScanLinesLabel.UseCompatibleTextRendering = true;
      // 
      // SanLinesTotal
      // 
      this.SanLinesTotal.BorderStyle = System.Windows.Forms.BorderStyle.None;
      this.SanLinesTotal.Enabled = false;
      this.SanLinesTotal.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SanLinesTotal.Location = new System.Drawing.Point(627, 28);
      this.SanLinesTotal.Name = "SanLinesTotal";
      this.SanLinesTotal.ReadOnly = true;
      this.SanLinesTotal.Size = new System.Drawing.Size(90, 13);
      this.SanLinesTotal.TabIndex = 15;
      this.SanLinesTotal.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // MouseCoordinates
      // 
      this.MouseCoordinates.BorderStyle = System.Windows.Forms.BorderStyle.None;
      this.MouseCoordinates.Enabled = false;
      this.MouseCoordinates.Font = new System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.MouseCoordinates.Location = new System.Drawing.Point(289, 79);
      this.MouseCoordinates.Name = "MouseCoordinates";
      this.MouseCoordinates.ReadOnly = true;
      this.MouseCoordinates.Size = new System.Drawing.Size(139, 15);
      this.MouseCoordinates.TabIndex = 16;
      this.MouseCoordinates.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // MouseCoordinatesLabel
      // 
      this.MouseCoordinatesLabel.AutoSize = true;
      this.MouseCoordinatesLabel.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.MouseCoordinatesLabel.Location = new System.Drawing.Point(262, 78);
      this.MouseCoordinatesLabel.Name = "MouseCoordinatesLabel";
      this.MouseCoordinatesLabel.Size = new System.Drawing.Size(23, 17);
      this.MouseCoordinatesLabel.TabIndex = 22;
      this.MouseCoordinatesLabel.Text = "X-Y";
      this.MouseCoordinatesLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.MouseCoordinatesLabel.UseCompatibleTextRendering = true;
      // 
      // AutoScrollStop
      // 
      this.AutoScrollStop.Location = new System.Drawing.Point(126, 5);
      this.AutoScrollStop.Name = "AutoScrollStop";
      this.AutoScrollStop.Size = new System.Drawing.Size(38, 23);
      this.AutoScrollStop.TabIndex = 31;
      this.AutoScrollStop.Text = "Stop";
      this.AutoScrollStop.UseCompatibleTextRendering = true;
      this.AutoScrollStop.UseVisualStyleBackColor = true;
      this.AutoScrollStop.Click += new System.EventHandler(this.AutoScrollStop_Click);
      // 
      // AutoScrollingRate
      // 
      this.AutoScrollingRate.BorderStyle = System.Windows.Forms.BorderStyle.None;
      this.AutoScrollingRate.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AutoScrollingRate.ForeColor = System.Drawing.SystemColors.WindowText;
      this.AutoScrollingRate.Location = new System.Drawing.Point(316, 10);
      this.AutoScrollingRate.Name = "AutoScrollingRate";
      this.AutoScrollingRate.ReadOnly = true;
      this.AutoScrollingRate.Size = new System.Drawing.Size(31, 13);
      this.AutoScrollingRate.TabIndex = 32;
      this.AutoScrollingRate.Text = "0";
      // 
      // AutoScrollDown
      // 
      this.AutoScrollDown.Location = new System.Drawing.Point(184, 5);
      this.AutoScrollDown.Name = "AutoScrollDown";
      this.AutoScrollDown.Size = new System.Drawing.Size(90, 23);
      this.AutoScrollDown.TabIndex = 31;
      this.AutoScrollDown.Text = "Scroll Down";
      this.AutoScrollDown.UseCompatibleTextRendering = true;
      this.AutoScrollDown.UseVisualStyleBackColor = true;
      this.AutoScrollDown.Click += new System.EventHandler(this.AutoScrollDown_Click);
      // 
      // AutoScrollUp
      // 
      this.AutoScrollUp.Location = new System.Drawing.Point(14, 5);
      this.AutoScrollUp.Name = "AutoScrollUp";
      this.AutoScrollUp.Size = new System.Drawing.Size(90, 23);
      this.AutoScrollUp.TabIndex = 30;
      this.AutoScrollUp.Text = "Scroll Up";
      this.AutoScrollUp.UseCompatibleTextRendering = true;
      this.AutoScrollUp.UseVisualStyleBackColor = true;
      this.AutoScrollUp.Click += new System.EventHandler(this.AutoScrollUp_Click);
      // 
      // autoDetect
      // 
      this.autoDetect.AutoSize = true;
      this.autoDetect.Location = new System.Drawing.Point(83, 79);
      this.autoDetect.Name = "autoDetect";
      this.autoDetect.Size = new System.Drawing.Size(60, 18);
      this.autoDetect.TabIndex = 34;
      this.autoDetect.Text = "Identify";
      this.autoDetect.UseCompatibleTextRendering = true;
      this.autoDetect.UseVisualStyleBackColor = true;
      this.autoDetect.CheckedChanged += new System.EventHandler(this.autoDetect_CheckedChanged);
      // 
      // colorize
      // 
      this.colorize.AutoSize = true;
      this.colorize.Location = new System.Drawing.Point(12, 79);
      this.colorize.Name = "colorize";
      this.colorize.Size = new System.Drawing.Size(65, 18);
      this.colorize.TabIndex = 35;
      this.colorize.Text = "Colorize";
      this.colorize.UseCompatibleTextRendering = true;
      this.colorize.UseVisualStyleBackColor = true;
      this.colorize.CheckedChanged += new System.EventHandler(this.colorize_CheckedChanged);
      // 
      // cropColLeft
      // 
      this.cropColLeft.BorderStyle = System.Windows.Forms.BorderStyle.None;
      this.cropColLeft.Enabled = false;
      this.cropColLeft.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.cropColLeft.Location = new System.Drawing.Point(15, 136);
      this.cropColLeft.Name = "cropColLeft";
      this.cropColLeft.ReadOnly = true;
      this.cropColLeft.Size = new System.Drawing.Size(39, 13);
      this.cropColLeft.TabIndex = 36;
      this.cropColLeft.Text = "0";
      // 
      // cropColRight
      // 
      this.cropColRight.BorderStyle = System.Windows.Forms.BorderStyle.None;
      this.cropColRight.Enabled = false;
      this.cropColRight.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.cropColRight.Location = new System.Drawing.Point(679, 136);
      this.cropColRight.Name = "cropColRight";
      this.cropColRight.ReadOnly = true;
      this.cropColRight.Size = new System.Drawing.Size(39, 13);
      this.cropColRight.TabIndex = 37;
      this.cropColRight.Text = "2047";
      this.cropColRight.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      // 
      // ScanLinesDisplayedLabel
      // 
      this.ScanLinesDisplayedLabel.AutoSize = true;
      this.ScanLinesDisplayedLabel.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ScanLinesDisplayedLabel.Location = new System.Drawing.Point(470, 77);
      this.ScanLinesDisplayedLabel.Name = "ScanLinesDisplayedLabel";
      this.ScanLinesDisplayedLabel.Size = new System.Drawing.Size(84, 17);
      this.ScanLinesDisplayedLabel.TabIndex = 38;
      this.ScanLinesDisplayedLabel.Text = "Lines Displayed";
      this.ScanLinesDisplayedLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.ScanLinesDisplayedLabel.UseCompatibleTextRendering = true;
      // 
      // ScanLinesDisplayed
      // 
      this.ScanLinesDisplayed.BorderStyle = System.Windows.Forms.BorderStyle.None;
      this.ScanLinesDisplayed.Enabled = false;
      this.ScanLinesDisplayed.Font = new System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ScanLinesDisplayed.Location = new System.Drawing.Point(557, 79);
      this.ScanLinesDisplayed.Name = "ScanLinesDisplayed";
      this.ScanLinesDisplayed.ReadOnly = true;
      this.ScanLinesDisplayed.Size = new System.Drawing.Size(159, 15);
      this.ScanLinesDisplayed.TabIndex = 39;
      this.ScanLinesDisplayed.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // AutoScrollingRateLabel
      // 
      this.AutoScrollingRateLabel.AutoSize = true;
      this.AutoScrollingRateLabel.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AutoScrollingRateLabel.Location = new System.Drawing.Point(293, 7);
      this.AutoScrollingRateLabel.Name = "AutoScrollingRateLabel";
      this.AutoScrollingRateLabel.Size = new System.Drawing.Size(19, 17);
      this.AutoScrollingRateLabel.TabIndex = 40;
      this.AutoScrollingRateLabel.Text = "lps";
      this.AutoScrollingRateLabel.TextAlign = System.Drawing.ContentAlignment.BottomRight;
      this.AutoScrollingRateLabel.UseCompatibleTextRendering = true;
      // 
      // ScrollingPannel
      // 
      this.ScrollingPannel.Controls.Add(this.AutoScrollingRate);
      this.ScrollingPannel.Controls.Add(this.AutoScrollingRateLabel);
      this.ScrollingPannel.Controls.Add(this.AutoScrollStop);
      this.ScrollingPannel.Controls.Add(this.AutoScrollDown);
      this.ScrollingPannel.Controls.Add(this.AutoScrollUp);
      this.ScrollingPannel.Location = new System.Drawing.Point(168, 117);
      this.ScrollingPannel.Name = "ScrollingPannel";
      this.ScrollingPannel.Size = new System.Drawing.Size(365, 32);
      this.ScrollingPannel.TabIndex = 41;
      // 
      // groundTruth
      // 
      this.groundTruth.AutoSize = true;
      this.groundTruth.Location = new System.Drawing.Point(12, 103);
      this.groundTruth.Name = "groundTruth";
      this.groundTruth.Size = new System.Drawing.Size(90, 18);
      this.groundTruth.TabIndex = 42;
      this.groundTruth.Text = "Ground Truth";
      this.groundTruth.UseCompatibleTextRendering = true;
      this.groundTruth.UseVisualStyleBackColor = true;
      // 
      // CounterFileViewer
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(210)))), ((int)(((byte)(210)))), ((int)(((byte)(255)))));
      this.ClientSize = new System.Drawing.Size(732, 715);
      this.Controls.Add(this.groundTruth);
      this.Controls.Add(this.ScanLinesDisplayed);
      this.Controls.Add(this.ScanLinesDisplayedLabel);
      this.Controls.Add(this.cropColRight);
      this.Controls.Add(this.cropColLeft);
      this.Controls.Add(this.colorize);
      this.Controls.Add(this.autoDetect);
      this.Controls.Add(this.MouseCoordinatesLabel);
      this.Controls.Add(this.MouseCoordinates);
      this.Controls.Add(this.SanLinesTotal);
      this.Controls.Add(this.TotalScanLinesLabel);
      this.Controls.Add(this.FileNameLabel);
      this.Controls.Add(this.FileName);
      this.Controls.Add(this.vScrollBar1);
      this.Controls.Add(this.ScannerPanel);
      this.Controls.Add(this.menuStrip1);
      this.Controls.Add(this.ScrollingPannel);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.KeyPreview = true;
      this.MainMenuStrip = this.menuStrip1;
      this.MinimumSize = new System.Drawing.Size(662, 747);
      this.Name = "CounterFileViewer";
      this.Text = "K-Square Flow-Thru File Viewer";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.CounterFileViewer_FormClosing);
      this.Load += new System.EventHandler(this.CounterFileViewer_Load);
      this.SizeChanged += new System.EventHandler(this.CounterFileViewer_SizeChanged);
      this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.CounterFileViewer_KeyDown);
      this.menuStrip1.ResumeLayout(false);
      this.menuStrip1.PerformLayout();
      this.ScrollingPannel.ResumeLayout(false);
      this.ScrollingPannel.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Panel ScannerPanel;
    private System.Windows.Forms.VScrollBar vScrollBar1;
    private System.Windows.Forms.TextBox FileName;
    private System.Windows.Forms.Label FileNameLabel;
    private System.Windows.Forms.MenuStrip menuStrip1;
    private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem openScannerFileToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
    private System.Windows.Forms.Timer StatusUpdateTimer;
    private System.Windows.Forms.Label TotalScanLinesLabel;
    private System.Windows.Forms.TextBox SanLinesTotal;
    private System.Windows.Forms.TextBox MouseCoordinates;
    private System.Windows.Forms.Label MouseCoordinatesLabel;
    private System.Windows.Forms.Button AutoScrollUp;
    private System.Windows.Forms.Button AutoScrollDown;
    private System.Windows.Forms.Button AutoScrollStop;
    private System.Windows.Forms.TextBox AutoScrollingRate;
    private System.Windows.Forms.CheckBox autoDetect;
    private System.Windows.Forms.ToolStripMenuItem autoDetectionToolStripMenuItem1;
    private System.Windows.Forms.CheckBox colorize;
    private System.Windows.Forms.TextBox cropColLeft;
    private System.Windows.Forms.TextBox cropColRight;
    private System.Windows.Forms.Label ScanLinesDisplayedLabel;
    private System.Windows.Forms.TextBox ScanLinesDisplayed;
    private System.Windows.Forms.Label AutoScrollingRateLabel;
    private System.Windows.Forms.Panel ScrollingPannel;
    private System.Windows.Forms.ToolStripMenuItem headerInfoToolStripMenuItem;
    private System.Windows.Forms.CheckBox groundTruth;
    private System.Windows.Forms.ToolStripMenuItem displayHeaderFieldsToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem cameraLineToolStripMenuItem;
  }
}

